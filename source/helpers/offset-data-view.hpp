#pragma once

#include "check-bounds.hpp"
#include <memory>
#include <vector>

namespace MdlParser {
  class OffsetDataView {
  public:
    explicit OffsetDataView(const std::weak_ptr<std::vector<std::byte>>& data);

    OffsetDataView(const OffsetDataView& from, const size_t offset);

    ~OffsetDataView() = default;
    OffsetDataView(const OffsetDataView&) = delete;
    OffsetDataView& operator=(const OffsetDataView&) = delete;
    OffsetDataView(const OffsetDataView&&) = delete;
    OffsetDataView& operator=(const OffsetDataView&&) = delete;

    template <typename T> [[nodiscard]] T parseStruct(const size_t relativeOffset, const char* errorMessage) const {
      const auto lockedData = getLockedData();
      const auto absoluteOffset = offset + relativeOffset;
      checkBounds(absoluteOffset, sizeof(T), lockedData->size(), errorMessage);

      return *reinterpret_cast<const T*>(&lockedData->at(offset));
    }

    template <typename T>
    [[nodiscard]] std::vector<T> parseStructArray(
      const size_t relativeOffset, const size_t count, const char* errorMessage
    ) const {
      const auto lockedData = getLockedData();
      const auto absoluteOffset = offset + relativeOffset;
      checkBounds(absoluteOffset, sizeof(T) * count, lockedData->size(), errorMessage);

      const T* first = reinterpret_cast<const T*>(&lockedData->at(offset));
      return std::vector<T>(first, first + count);
    }

    std::string parseString(const size_t relativeOffset, const char* errorMessage) const;

  private:
    const std::weak_ptr<std::vector<std::byte>> data;
    const size_t offset;

    [[nodiscard]] std::shared_ptr<std::vector<std::byte>> getLockedData() const;
  };
}
