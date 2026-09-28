#include "test.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <random>
#include <sstream>

using std::default_random_engine;
using std::generate;
using std::hex;
using std::max;
using std::ostringstream;
using std::string;
using std::vector;

#define MAX_STACK_SIZE 1000000
#define SMALL_STACK_SIZE 256

static char binary_ops[] = "+*^|&";
static constexpr size_t binary_ops_count = sizeof(binary_ops) - 1;
static char program_200[] =
    "0=1=2=3=4n+Z5=6=7=8=9=X-+Y+*7*1n-+W2XY+X1-+7=5=+-+*-^|+*-^|";

TestChunk generate_many_debug_chunk(size_t call_count, size_t ops_per_call,
                                    string& data,
                                    default_random_engine& engine) {
  data = "1=2=3=";
  size_t stack_size = 3;
  vector<DebugCall> calls;
  for (size_t i = 0; i < call_count; i++) {
    DebugCall& call = calls.emplace_back();
    data += 'g';
    for (size_t j = 0; j < ops_per_call; j++) {
      switch (engine() % 4) {
        case 0: {
          size_t count = engine() % max<size_t>(1, stack_size);
          call.emplace_back(DebugPop{count});
          stack_size -= count;
          break;
        }
        case 1: {
          if (stack_size >= SMALL_STACK_SIZE) {
            break;
          }
          size_t count = engine() % (SMALL_STACK_SIZE - stack_size);
          vector<uint64_t> values(count);
          generate(values.begin(), values.end(), [&] { return engine(); });
          call.emplace_back(DebugPush{std::move(values)});
          stack_size += count;
          break;
        }
        case 2: {
          size_t offset = engine() % (stack_size + 1);
          size_t count = engine() % (stack_size + 1 - offset);
          vector<uint64_t> values(count);
          generate(values.begin(), values.end(), [&] { return engine(); });
          call.emplace_back(DebugSet{offset, std::move(values)});
          break;
        }
        case 3: {
          break;
        }
        default:
          assert(false);
      }
    }
    size_t reduce_count = engine() % max<size_t>(1, stack_size);
    for (size_t j = 0; j < reduce_count; j++) {
      data += binary_ops[engine() % binary_ops_count];
      stack_size--;
    }
  }

  return {data.c_str(), 0, std::move(calls)};
}

void generate_stack_addition(string& data, size_t part_size) {
  data.resize(part_size * 3 - 1);

  const char* pattern = "1=1=1=1=";
  const size_t pattern_per_uint = 4;
  const auto* pattern_ptr = reinterpret_cast<const uint64_t*>(pattern);

  assert(part_size % pattern_per_uint == 0);
  auto* data_ptr = reinterpret_cast<uint64_t*>(data.data());

  for (size_t i = 0; i < part_size / pattern_per_uint; i++) {
    *data_ptr = *pattern_ptr;
    data_ptr++;
  }

  memset(data.data() + part_size * sizeof(*pattern_ptr) / pattern_per_uint, '+',
         part_size - 1);
}

void require_threads(size_t thread_count) {
  if (thread_count > N) {
    throw NoSuchTest();
  }
}

Test multiply_chunks(Test test, size_t multiplier) {
  for (auto& thread : test) {
    assert(thread.size() == 1);
    thread = vector<TestChunk>(multiplier, thread[0]);
  }
  return test;
}

Test get_test(size_t num) {
  static vector<string> data;
  data.clear();
  // Stałe ziarno, żeby zawsze generować taki sam test.
  default_random_engine engine(0x123);
  switch (num) {
    case 100: {
      require_threads(1);
      // Uruchamia podstawowy test.
      return {{{"1", 1, {}}}};
    }
    case 101: {
      require_threads(1);
      // Testuje operacje arytmetyczne.
      return {{
          {"123=456+", 0x123 + 0x456, {}},
          {"123=456*", 0x123 * 0x456, {}},
          {"123=456-", static_cast<uint64_t>(-0x456), {}},
          {"123=456&", 0x123 & 0x456, {}},
          {"123=456|", 0x123 | 0x456, {}},
          {"123=456^", 0x123 ^ 0x456, {}},
          {"123=456~", static_cast<uint64_t>(~0x456), {}},
          {"123=456X", 0x123, {}},
          {"123=456Z", 0x123, {}},
          {"123=456YZ", 0x456, {}},
          {"123=456N", N, {}},
          {"123=456n", 0, {}},
      }};
    }
    case 102: {
      require_threads(1);
      // Testuje alokowanie całego stosu.
      data.resize(1);
      generate_stack_addition(data[0], MAX_STACK_SIZE);
      return {{
          {data[0].c_str(), MAX_STACK_SIZE, {}},
      }};
    }
    case 103: {
      require_threads(1);
      // Testuje debug dokładające na stos.
      return {{{"g", 0x123, {{DebugPush{0x123}}}}}};
    }
    case 104: {
      require_threads(1);
      // Testuje debug zdejmujące ze stosu.
      return {{
          {"123=456=g",
           0x123,
           {
               {
                   DebugCheck{0, 0x456, 0x123},
                   DebugPop{0x1},
               },
           }},
      }};
    }
    case 105: {
      require_threads(1);
      // Testuje zmianę stosu.
      return {{
          {"123=456=g+",
           0x456 + 0x789,
           {
               {
                   DebugCheck{0, 0x456, 0x123},
                   DebugSet{1, 0x789},
               },
           }},
          {"123=456=g",
           0x789,
           {
               {
                   DebugCheck{0, 0x456, 0x123},
                   DebugSet{0, 0x1, 0x789},
                   DebugPop{1},
               },
           }},
      }};
    }
    case 106: {
      require_threads(1);
      // Testuje debug modyfikujące stos.
      const size_t call_count = 1000;
      const size_t ops_per_call = 4;

      data.resize(4);

      Test result = {{
          generate_many_debug_chunk(call_count, ops_per_call, data[0], engine),
          generate_many_debug_chunk(call_count, ops_per_call, data[1], engine),
          generate_many_debug_chunk(call_count, ops_per_call, data[2], engine),
          generate_many_debug_chunk(call_count, ops_per_call, data[3], engine),
      }};
      result[0][0].result = 0xc24e412a2f058e53;
      result[0][1].result = 0xde2168d067ae16a7;
      result[0][2].result = 0x3fe5d676;
      result[0][3].result = 0x9571db59e5b56827;
      return result;
    }
    case 107: {
      require_threads(1);
      // Testuje wczytywanie liczb.
      return {{
          {"3824789472974924", 0x3824789472974924, {}},
          {"3824789472974924123133", 0x9472974924123133, {}},
          {"abcdefABCDEF", 0xabcdefabcdef, {}},
          {"abcdefABCDEFabcdefABCDEF", 0xcdefabcdefabcdef, {}},
      }};
    }
    case 200: {
      require_threads(2);
      // Uruchamia skomplikowane obliczenie.
      return {
          {
              {program_200, 0xffffffffffffef1d, {}},
          },
          {
              {program_200, 0xffffffffffffef1d, {}},
          },
      };
    }
    case 201: {
      require_threads(2);
      // Dużo synchronizacji.
      data.resize(2);
      data[0] = "123=";
      data[1] = "456=";
      const size_t round_size = 1000;
      size_t round_count = 50;
      string rounds[2];
      for (size_t i = 0; i < round_size - 1; i++) {
        // To numery drugiego notecia, ale przed użyciem zostaną zamienione.
        rounds[0] += "0=";
        rounds[1] += "1=";
      }
      // Początkowe numery drugiego notecia.
      rounds[0] += "1=";
      rounds[1] += "0=";
      // Czekanie.
      for (size_t i = 0; i < round_size; i++) {
        rounds[0] += 'W';
        rounds[1] += 'W';
      }
      // Pomieszać wartości z numerem notecia, żeby wykryć błędy w zamianach.
      string end_mangle = "n+3*";
      rounds[0] += end_mangle;
      rounds[1] += end_mangle;
      for (size_t i = 0; i < round_count; i++) {
        data[0] += rounds[0];
        data[1] += rounds[1];
      }
      return {
          {{data[0].c_str(), 0x69381defee2b8dbc, {}}},
          {{data[1].c_str(), 0xf5d0b3847ad4def1, {}}},
      };
    }
    case 300: {
      require_threads(3);
      // Uruchamia prosty test z trzema noteciami.
      return multiply_chunks(
          {
              {
                  {"4=1W2W", 6, {}},
              },
              {
                  {"5=0W2W", 5, {}},
              },
              {
                  {"6=0W1W", 4, {}},
              },
          },
          0x1000);
    }
    case 301: {
      require_threads(3);
      // Uruchamia obliczenia zajmujące po jednej trzeciej stosu.
      size_t part_size = MAX_STACK_SIZE / 3;
      part_size &= ~0x3;

      data.resize(1);
      generate_stack_addition(data[0], part_size);

      return multiply_chunks(
          {
              {
                  {data[0].c_str(), part_size, {}},
              },
              {
                  {data[0].c_str(), part_size, {}},
              },
              {
                  {data[0].c_str(), part_size, {}},
              },

          },
          4);
    }
    case 302: {
      require_threads(3);
      // Test z treści.
      return {
          {
              {"nY1^W", 1, {}},
          },
          {
              {"nY1^W", 0, {}},
          },
          {
              {"6N8ZXab=12-+3*~FFF&cDe09|g",
               6,
               {
                   {
                       DebugCheck(0, (~((0xab - 0x12) * 3) & 0xfff) | 0xcde09),
                       DebugPop(1),
                   },
               }},
          },
      };
    }
    case 400: {
      require_threads(2);
      // Bardzo dużo kooperatywnej wymiany.
      data.resize(1);
      data[0] = "nn*";
      size_t count = 1;
      while (count * 2 <= N) {
        count *= 2;
      }

      // Zamieniamy się z każdym innym noteciem, co nie zmienia stanu dla
      // przystej liczby wykonań.
      const size_t rounds = 600;
      assert(rounds % 2 == 0);
      for (size_t round = 0; round < rounds; round++) {
        for (size_t i = 1; i < count; i++) {
          ostringstream sstream;
          sstream << hex << i;
          data[0] += sstream.str();
          data[0] += "n^W";
        }
      }

      // Dodatkowa zamiana, żeby końcowy stan był inny od początkowego.
      data[0] += "1n^W";

      Test result;
      for (size_t i = 0; i < count; i++) {
        TestChunk chunk{data[0].c_str(), (i ^ 1) * (i ^ 1), {}};
        result.emplace_back(TestThread{chunk});
      }

      return multiply_chunks(result, 96);
    }
    case 401: {
      // Bardzo dużo niekooperatywnej wymiany.
      require_threads(2);
      data.resize(3);
      data[0] = "nn*";  // Kod dla notecia 0.
      data[1] = "nn*";  // Kod dla noteci od 1 do N-2.
      data[2] = "nn*";  // Kod dla notecia N-1.

      // W tym teście robimy obrót cykliczny rounds razy.
      // Ponieważ rounds == (-1) [mod N], to otrzymamy wartość z notecia o
      // numerze o jeden większym.
      const size_t rounds = 350 * N + N - 1;
      for (size_t round = 0; round < rounds; round++) {
        data[0] += "1W";  // Zamienia tylko z większym (czyli 1).
        data[1] +=
            "n1+Wn1-+W";     // Zamienia się z większym, a potem z mniejszym.
        data[2] += "N2-+W";  // Zamienia się tylko z mniejszym (czyli N + (-2)).
      }

      Test result;
      for (size_t i = 0; i < N; i++) {
        const char* calc;
        if (i == 0) {
          calc = data[0].c_str();
        } else if (i == N - 1) {
          calc = data[2].c_str();
        } else {
          calc = data[1].c_str();
        }
        size_t exp_i = (i + 1) % N;
        TestChunk chunk{calc, exp_i * exp_i, {}};
        result.emplace_back(TestThread{chunk});
      }

      return multiply_chunks(result, 96);
    }

    default:
      throw NoSuchTest();
  }
}
