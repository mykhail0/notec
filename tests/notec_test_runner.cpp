#include "notec_test_runner.hpp"

#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cassert>
#include <csignal>
#include <cstring>
#include <memory>
#include <optional>
#include <sstream>
#include <thread>
#include <variant>
#include <vector>

using std::bad_variant_access;
using std::exception;
using std::exception_ptr;
using std::get;
using std::get_if;
using std::make_exception_ptr;
using std::make_shared;
using std::max;
using std::optional;
using std::ostringstream;
using std::rethrow_exception;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::thread;
using std::variant;
using std::vector;

extern "C" [[noreturn]] void abort_thread(int) {
  pthread_exit(nullptr);  // Teoretycznie niedozwolone.
}

#define BROKEN_DEBUG_STACK 1
int broken_abi = 0;

using result_type = optional<variant<result_duration, exception_ptr>>;

extern "C" {
uint64_t notec_stub(uint32_t n, const char* calc, void* notec_stack,
                    void* debug_stack);
[[maybe_unused]] int64_t debug_stub(uint32_t n, uint64_t* stack_pointer);
}

struct test_runner {
  size_t debug_idx;
  const vector<DebugCall>* debugs;
  result_type* result;
  uint32_t id;

  template <typename T>
  static inline void append_to_ostream(std::ostream& ostream, T&& data) {
    ostream << std::forward<T>(data);
  }

  template <typename... Ts>
  [[noreturn]] void exit_with_message(Ts&&... args) const {
    ostringstream sstream;
    (append_to_ostream(sstream, std::forward<Ts>(args)), ...);
    *result = make_exception_ptr(FailedTest(sstream.str()));
    abort_thread(0);
  }

  int64_t debug(uint32_t n, uint64_t* stack_void_pointer) {
    auto stack_pointer = (uint64_t*)stack_void_pointer;
    if (n != id) {
      exit_with_message("Bad notec id passed to debug. Expected: ", id,
                        ", got: ", n);
    }
    if (debug_idx >= debugs->size()) {
      exit_with_message("Too many debug calls. Expected: ", debug_idx);
    }

    const auto& debug = debugs->at(debug_idx);

    auto* new_stack_pointer = stack_pointer;

    for (const auto& op : debug) {
      if (auto* pop = get_if<DebugPop>(&op)) {
        new_stack_pointer += pop->count;
      } else if (auto* push = get_if<DebugPush>(&op)) {
        new_stack_pointer -= push->values.size();
        memcpy(new_stack_pointer, push->values.data(),
               push->values.size() * sizeof(push->values[0]));
      } else if (auto* check = get_if<DebugCheck>(&op)) {
        for (size_t i = 0; i < check->values.size(); i++) {
          if (new_stack_pointer[check->offset + i] != check->values[i]) {
            exit_with_message("Bad value on stack at position: ", i,
                              " at call_idx: ", debug_idx,
                              " expected: ", check->values[i],
                              " got: ", new_stack_pointer[check->offset + i]);
          }
        }
      } else if (auto* set = get_if<DebugSet>(&op)) {
        for (size_t i = 0; i < set->values.size(); i++) {
          new_stack_pointer[set->offset + i] = set->values[i];
        }
      } else {
        throw bad_variant_access();
      }
    }

    debug_idx++;
    return static_cast<int64_t>(new_stack_pointer - stack_pointer);
  }

  void run(const TestChunk& test, void* notec_stack, void* debug_stack) {
    debug_idx = 0;
    debugs = &test.debugs;
    uint64_t val = notec_stub(id, test.calc, notec_stack, debug_stack);
    if (debug_idx != debugs->size()) {
      exit_with_message("Not enough debug calls. Expected: ", debugs->size(),
                        ", got: ", debug_idx);
    } else if (val != test.result) {
      exit_with_message("Wrong result. Expected: ", test.result,
                        ", got: ", val);
    }
  }

  void run(size_t _id, const TestThread& test, result_type* _result,
           void* notec_stack, void* debug_stack) {
    id = _id;
    result = _result;

    auto start = std::chrono::steady_clock::now();
    for (const auto& chunk : test) {
      run(chunk, notec_stack, debug_stack);
    }
    auto end = std::chrono::steady_clock::now();
    *result = end - start;
  }
};

template <size_t SIZE>
class extra_stack {
 private:
  static constexpr size_t PAGE_SIZE = 0x1000;
  static constexpr size_t total_size = SIZE + PAGE_SIZE * 2;

  uint8_t* ptr = nullptr;

  void unmap() noexcept {
    if (ptr) {
      munmap(ptr, total_size);
      ptr = nullptr;
    }
  }

 public:
  extra_stack() {
    static_assert(SIZE % PAGE_SIZE == 0,
                  "Stack size not divisible by page size.");
    if (getpagesize() != PAGE_SIZE) {
      throw std::runtime_error("Bad page size.");
    }

    uint8_t* new_ptr = (uint8_t*)mmap(NULL, total_size, PROT_WRITE | PROT_READ,
                                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (new_ptr == MAP_FAILED) {
      throw std::bad_alloc();
    }

    ptr = new_ptr;

    if (mprotect(ptr, PAGE_SIZE, PROT_NONE)) {
      unmap();
      throw std::runtime_error("Can't set mprotect." + string(strerror(errno)));
    }
    if (mprotect(ptr + PAGE_SIZE + SIZE, PAGE_SIZE, PROT_NONE)) {
      unmap();
      throw std::runtime_error("Can't set mprotect." + string(strerror(errno)));
    }
  }

  ~extra_stack() noexcept { unmap(); }

  void* start_ptr() {
    assert(ptr);
    return ptr + PAGE_SIZE;
  }

  void* end_ptr() {
    assert(ptr);
    return ptr + PAGE_SIZE + SIZE;
  }
};

class sigaction_updater {
 private:
  int sig;
  struct sigaction original;

 public:
  sigaction_updater(int sig_num, void handler(int)) : sig(sig_num) {
    struct sigaction action;
    action.sa_handler = handler;
    action.sa_flags = SA_ONSTACK;
    sigfillset(&action.sa_mask);

    if (sigaction(sig, &action, &original)) {
      throw FailedTest("Can't set sigaction for signal.",
                       FailedTest::Type::TEST_ERROR);
    }
  }

  ~sigaction_updater() noexcept { sigaction(sig, &original, nullptr); }
};

template <size_t SIZE>
class signal_stack_updater {
 private:
  extra_stack<SIZE> stack_data;
  stack_t original;

 public:
  signal_stack_updater() {
    stack_t signal_stack{
        .ss_sp = stack_data.start_ptr(),
        .ss_flags = SS_ONSTACK,
        .ss_size = SIZE,
    };

    if (sigaltstack(&signal_stack, &original)) {
      throw std::runtime_error("Can't set alternative signal stack.");
    }
  }

  ~signal_stack_updater() noexcept {
    if (sigaltstack(&original, nullptr)) {
      exit(1);
    }
  }
};

static thread_local test_runner runner;

[[maybe_unused]] int64_t debug_stub(uint32_t n, uint64_t* stack_pointer) {
  auto result = runner.debug(n, stack_pointer);

  return result;
}

void run_thread(size_t id, const TestThread& test,
                shared_ptr<result_type> result) {
  try {
    signal_stack_updater<1024 * 1024 * 10> update_signal_stack;

    sigaction_updater update_sigsegv(SIGSEGV, abort_thread);
    sigaction_updater update_sigill(SIGILL, abort_thread);
    sigaction_updater update_sigfpe(SIGFPE, abort_thread);

    extra_stack<1024 * 1024 * sizeof(uint64_t)> notec_stack;
    extra_stack<1024 * 1024> debug_stack;

    runner.run(id, test, result.get(), notec_stack.end_ptr(),
               debug_stack.end_ptr());
  } catch (const FailedTest& e) {
    *result = make_exception_ptr(e);
    return;
  } catch (const std::exception& e) {
    *result =
        make_exception_ptr(FailedTest(e.what(), FailedTest::Type::TEST_ERROR));
    return;
  }
}

result_duration run_test(const Test& test) {
  vector<shared_ptr<result_type>> results;
  for (size_t i = 0; i < test.size(); i++) {
    results.emplace_back(make_shared<result_type>());
  }

  vector<thread> threads;
  threads.reserve(test.size());

  for (size_t i = 0; i < test.size(); i++) {
    threads.emplace_back(run_thread, i, test[i], results[i]);
  }

  result_duration result(0);
  for (size_t i = 0; i < test.size(); i++) {
    threads[i].join();
    auto thread_result = results[i]->value_or(make_exception_ptr(
        FailedTest("Run crashed.", FailedTest::Type::CRASH)));
    if (auto* e = get_if<exception_ptr>(&thread_result)) {
      for (size_t j = i + 1; j < threads.size(); j++) {
        threads[j].detach();
      }
      rethrow_exception(*e);
    } else {
      result = max(result, get<result_duration>(thread_result));
    }
  }
  if (broken_abi) {
    throw FailedTest("Broken abi.", FailedTest::Type::ABI);
  }
  return result;
}
