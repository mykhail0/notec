#pragma once

#include <cstdint>
#include <cstdlib>
#include <variant>
#include <vector>

struct DebugPop {
 public:
  uint64_t count;

  constexpr explicit DebugPop(uint64_t _value) : count(_value) {}
};

struct DebugCheck {
 public:
  size_t offset;
  std::vector<uint64_t> values;

  template <typename... Ts>
  constexpr explicit DebugCheck(uint64_t _offset, Ts... args)
      : offset(_offset), values{static_cast<uint64_t>(args)...} {}

  explicit DebugCheck(uint64_t _offset, std::vector<uint64_t>&& _values)
      : offset(_offset), values(std::move(_values)) {}
};

struct DebugSet {
 public:
  size_t offset;
  std::vector<uint64_t> values;

  template <typename... Ts>
  constexpr explicit DebugSet(uint64_t _offset, Ts... args)
      : offset(_offset), values{static_cast<uint64_t>(args)...} {}

  explicit DebugSet(uint64_t _offset, std::vector<uint64_t>&& _values)
      : offset(_offset), values(std::move(_values)) {}
};

struct DebugPush {
 public:
  std::vector<uint64_t> values;

  DebugPush(std::initializer_list<uint64_t> _values) : values(_values) {}

  explicit DebugPush(std::vector<uint64_t>&& _values)
      : values(std::move(_values)) {}
};

using DebugOperation = std::variant<DebugPop, DebugPush, DebugCheck, DebugSet>;

using DebugCall = std::vector<DebugOperation>;

struct TestChunk {
  const char* calc;
  uint64_t result;
  std::vector<DebugCall> debugs;

  TestChunk(const char* _calc, uint64_t _result,
            std::initializer_list<DebugCall> _debugs)
      : calc(_calc), result(_result), debugs(_debugs) {}

  TestChunk(const char* _calc, uint64_t _result,
            std::vector<DebugCall>&& _debugs)
      : calc(_calc), result(_result), debugs(std::move(_debugs)) {}
};

using TestThread = std::vector<TestChunk>;

using Test = std::vector<TestThread>;
