#pragma once

#include "structs/vvd.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace MdlParser {
  class Vvd {
  public:
    /**
     * Parses a .vvd file contained in the given buffer into an easier to use and more modern structure.
     * No ownership of the data is taken as all contents are copied into new structs.
     *
     * @param data
     * @param checksum Optional checksum to validate against the header's
     */
    explicit Vvd(
      const std::weak_ptr<std::vector<std::byte>>& data, const std::optional<int32_t>& checksum = std::nullopt
    );

    /**
     * Gets the checksum shared by the MDL, VTX and VVD from the header.
     * @remarks Can be used to loosely verify that a collection of MDL, VTX and VVD files were compiled from the same asset.
     * @return int32_t checksum
     */
    [[nodiscard]] int32_t getChecksum() const;

    [[nodiscard]] const std::vector<Structs::Vvd::Vertex>& getVertices() const;
    [[nodiscard]] const std::vector<Structs::Vector4D>& getTangents() const;
    [[nodiscard]] int32_t getNumLods() const;

  private:
    Structs::Vvd::Header header;
    std::vector<Structs::Vvd::Vertex> vertices;
    std::vector<Structs::Vector4D> tangents;

    int32_t numLods = 0;
  };
}
