#pragma once

#include "../errors.hpp"
#include <vector>

namespace MdlParser {
  template <typename T>
  T parseStruct(const std::vector<std::byte>& data, const size_t offset, const char* errorMessage) {
    if (offset + sizeof(T) > data.size()) {
      throw Errors::OutOfBoundsAccess(errorMessage);
    }

    return *reinterpret_cast<const T*>(&data[offset]);
  }

  template <typename T>
  std::vector<T> parseStructArray(
    const std::vector<std::byte>& data, const size_t offset, const size_t count, const char* errorMessage
  ) {
    if (offset + sizeof(T) * count > data.size()) {
      throw Errors::OutOfBoundsAccess(errorMessage);
    }

    const T* first = reinterpret_cast<const T*>(&data[offset]);
    return std::vector<T>(first, first + count);
  }
}
