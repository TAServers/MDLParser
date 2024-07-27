#pragma once

#include <string>
#include <vector>

namespace MdlParser {
  std::string parseString(const std::vector<std::byte>& data, size_t offset);
}
