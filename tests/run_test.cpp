#include <iostream>

#include "notec_test_runner.hpp"
#include "test.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <test_num>" << std::endl;
    exit(1);
  }
  size_t test_num = std::stoull(argv[1]);
  try {
    auto time = run_test(get_test(test_num));
    (void)time;
    std::cout << "TEST SUCCEEDED" << std::endl;
    std::cout << "TIME: "
              << std::chrono::duration_cast<
                     std::chrono::duration<double, std::milli>>(time)
                     .count()
              << " ms" << std::endl;
  } catch (const FailedTest& e) {
    std::cout << "FAILED: ";
    switch (e.get_type()) {
      case FailedTest::Type::CORRECTNESS: {
        std::cout << "CORRECTNESS";
        break;
      }
      case FailedTest::Type::ABI: {
        std::cout << "ABI" << " " << broken_abi;
        break;
      }
      case FailedTest::Type::CRASH: {
        std::cout << "CRASH";
        break;
      }
      case FailedTest::Type::TEST_ERROR: {
        std::cout << "TEST_ERROR";
        break;
      }
    }
    std::cout << std::endl;

    std::cerr << "FAILED: " << e.what() << std::endl;
    exit(1);
  } catch (const NoSuchTest& e) {
    std::cerr << "NO SUCH TEST NUMBER " << test_num << std::endl;
    exit(1);
  }
  return 0;
}
