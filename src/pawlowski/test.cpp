#include <vector>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <random>
#include <iostream>
#include <chrono>


extern "C" uint64_t notec(uint32_t n, char const *calc);
extern "C" int64_t debug(uint32_t n, uint64_t *stack_pointer) {
    (void)n;
    (void)stack_pointer;
    return 0;
}

unsigned int checkTestActualDep(char *x) {
    unsigned int len = strlen(x);
    std::vector<unsigned int> refs;
    std::vector<unsigned int> stack;
    bool enterMode = false;
    refs.resize((len + 1) * 2);
    for (unsigned int i = 1; *x; ++x, ++i) {
        char v = *x;
        if ((v >= '0' && v <= '9') || (v >= 'a' && v <= 'f') || (v >= 'A' && v <= 'F')) {
            if (!enterMode)
                stack.push_back(i);
            if (enterMode) {
                refs[2 * i] = i - 1;
                stack.back() = i;
            }
            enterMode = true;
            continue;
        }
        if (v == '=' && enterMode) { // count = as an okay character
            refs[2 * i] = i - 1;
            stack.back() = i;
        }
        enterMode = false;
        if (v == '+' || v == '*' || v == '&' || v == '|' || v == '^') {
            if (stack.size() < 2)
                throw std::runtime_error("Stack does not have 2 elements!");
            refs[2 * i] = stack.back();
            stack.pop_back();
            refs[2 * i + 1] = stack.back();
            stack.back() = i;
        } else if (v == '-' || v == '~') {
            if (stack.empty())
                throw std::runtime_error("Stack is empty!");
            refs[2 * i] = stack.back();
            stack.back() = i;
        } else if (v == 'Z') {
            if (stack.empty())
                throw std::runtime_error("Stack is empty!");
            stack.pop_back();
        } else if (v == 'Y') {
            if (stack.empty())
                throw std::runtime_error("Stack is empty!");
            refs[2 * i] = stack.back();
            stack.push_back(i);
        } else if (v == 'X') {
            if (stack.size() < 2)
                throw std::runtime_error("Stack does not have 2 elements!");
            std::swap(stack[stack.size() - 1], stack[stack.size() - 2]);
        } else if (v == 'N' || v == 'n') {
            stack.push_back(i);
        }
    }
    if (stack.empty())
        throw std::runtime_error("Stack is empty at the end of the operation!");

    std::vector<bool> result;
    result.resize(len + 1);
    result[stack.back()] = true;
    unsigned int resultNum = 0;
    for (unsigned int i = len; i >= 1; --i) {
        if (result[i]) {
            result[refs[2 * i]] = true;
            result[refs[2 * i + 1]] = true;
            ++resultNum;
        }
    }
    return resultNum;
}

int main() {
    const int stackSizeLimit = 512 * 1024;
    size_t size;

    uint64_t seed;
    std::cin >> size >> seed;

    std::mt19937_64 rnd (seed);

    char *buf = new char[size + 1];

    uint64_t aggMul1 = 1 + rnd() % 3;
    uint64_t aggBase = rnd() % 10;
    uint64_t aggMul = 1 + rnd() % 7;

    int stackSize = 0;
    bool enterMode = false;
    for (size_t i = 0; i < size; i++) {
        uint64_t wNumber      = 400000;
        uint64_t wDebug       = 10000;
        uint64_t wPushIndex   = 10000;
        uint64_t wPopDiscard  = 1000;
        uint64_t wDuplicate   = 100000;
        uint64_t wSingleArgOp = 100000;
        uint64_t wTwoArgOp    = 200000;
        uint64_t wSwap        = 100000;

        if (stackSize < 1) wPopDiscard = 0;
        if (stackSize < 1) wDuplicate = 0;
        if (stackSize < 1) wSingleArgOp = 0;
        if (stackSize < 2) wTwoArgOp = 0;
        if (stackSize < 2) wSwap = 0;

        if (i == size - 1) wPopDiscard = 0;
        if (i == size - 1 && stackSize == 0) wDebug = 0;

        if (stackSize > stackSizeLimit) {
            wPushIndex = 0;
            wDuplicate = 0;
            if (!enterMode)
                wNumber = 0;
        }

        // If we are nearing the end of the string, with a full stack, make the pop operators more likely
        int64_t mod = stackSize * aggMul1 - (size - i);
//        printf("%li [%i] %ld\n", i, stackSize, mod);
        if (mod > 0) {
            mod += aggBase;
            mod += aggMul;
            wTwoArgOp = wTwoArgOp * std::min<int64_t>(mod, 10000);
            wNumber = wNumber / std::min<int64_t>(mod, 100);
            wPushIndex = wPushIndex / std::min<int64_t>(mod, 50);
        }

        uint64_t wSum = wNumber + wDebug + wPushIndex + wPopDiscard + wDuplicate + wSingleArgOp + wTwoArgOp + wSwap;
        std::uniform_int_distribution<uint64_t> distrib(0, wSum - 1);
        auto v = (int64_t) distrib(rnd);

        if ((v -= wNumber) < 0) {
            buf[i] = "0123456789abcdefABCDEF"[rnd() % 22];
            if (!enterMode) {
                stackSize++;
                enterMode = true;
            }
            continue;
        }

        enterMode = false;
        if ((v -= wDebug) < 0) {
            buf[i] = 'g';
        } else if ((v -= wPushIndex) < 0) {
            buf[i] = "Nn"[rnd() % 2];
            ++stackSize;
        } else if ((v -= wPopDiscard) < 0) {
            buf[i] = 'Z';
            --stackSize;
        } else if ((v -= wDuplicate) < 0) {
            buf[i] = 'Y';
            ++stackSize;
        } else if ((v -= wSingleArgOp) < 0) {
            buf[i] = "-~"[rand() % 2];
        } else if ((v -= wTwoArgOp) < 0) {
            buf[i] = "+*&|^"[rand() % 5];
            --stackSize;
        } else if ((v -= wSwap) < 0) {
            buf[i] = 'X';
        } else {
            abort();
        }
    }
    buf[size] = 0;
    if (size <= 1024)
        std::cerr << "Test: " << buf << std::endl;
//    std::cerr << "Number of characters actually used in the test: " << checkTestActualDep(buf) << "/" << size << std::endl;

    auto begin = std::chrono::steady_clock::now();
    auto result = notec(0, buf);
    auto end = std::chrono::steady_clock::now();
    std::cerr << "Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(end - begin).count() << "s" << std::endl;

    std::cout << std::hex << result;

    return 0;
}