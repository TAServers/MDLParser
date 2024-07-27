#pragma once

#include "check-bounds.hpp"
#include "../errors.hpp"
#include <vector>

namespace MdlParser {
  template <typename T>
  T parseStruct(const std::vector<std::byte>& data, const size_t offset, const char* errorMessage) {
    checkBounds(offset, sizeof(T), data.size(), errorMessage);

    return *reinterpret_cast<const T*>(&data[offset]);
  }

  template <typename T>
  std::vector<T> parseStructArray(
    const std::vector<std::byte>& data, const size_t offset, const size_t count, const char* errorMessage
  ) {
    checkBounds(offset, sizeof(T) * count, data.size(), errorMessage);

    const T* first = reinterpret_cast<const T*>(&data[offset]);
    return std::vector<T>(first, first + count);
  }
}
