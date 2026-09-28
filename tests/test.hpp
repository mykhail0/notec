#pragma once

#include "test_object.hpp"

#include <exception>

Test get_test(size_t num);

class NoSuchTest : public std::exception {
public:
    [[nodiscard]] const char *what() const noexcept override {
        return "Bad test number.";
    }
};
