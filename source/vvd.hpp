#pragma once

#include "structs/vvd.hpp"
#include <vector>

namespace MdlParser {
  class Vvd {
  private:
    std::vector<Structs::Vvd::Vertex> vertices;
    std::vector<Structs::Vector4D> tangents;

    int32_t numLods = 0;

  public:
    Vvd(std::vector<std::byte>& data, const int32_t checksum);

    [[nodiscard]] const std::vector<Structs::Vvd::Vertex>& getVertices() const;
    [[nodiscard]] const std::vector<Structs::Vector4D>& getTangents() const;
    [[nodiscard]] int32_t getNumLods() const;
  };
}
