#include "parse-string.hpp"
#include "errors.hpp"

namespace MdlParser {
  using namespace Errors;

  std::string parseString(const std::vector<std::byte>& data, size_t offset, const char* errorMessage) {
    for (size_t i = offset; i < data.size(); i++) {
      if (data[i] == std::byte(0)) {
        return reinterpret_cast<const char*>(&data[offset]);
      }
    }

    throw OutOfBoundsAccess(errorMessage);
  }
}
