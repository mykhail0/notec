#pragma once

#include "test_object.hpp"

#include <exception>
#include <string>
#include <chrono>

using result_duration = std::chrono::duration<double>;

result_duration run_test(const Test &test);

extern int broken_abi;

class FailedTest : public std::exception {
public:
    enum class Type {
        CORRECTNESS,
        CRASH,
        ABI,
        TEST_ERROR,
    };
private:
    std::string message;
    Type type;
public:
    explicit FailedTest(const char *_message, Type _type = Type::CORRECTNESS) : message(_message), type(_type) {}
    explicit FailedTest(std::string &&_message, Type _type = Type::CORRECTNESS) : message(std::move(_message)), type(_type) {}

    [[nodiscard]] Type get_type() const noexcept {
        return type;
    }

    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }
};
