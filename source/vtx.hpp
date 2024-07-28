#pragma once

#include "enums.hpp"
#include "structs/vtx.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace MdlParser {
  class Vtx {
  public:
    struct Strip {
      int32_t verticesCount;
      int32_t verticesOffset;

      int32_t indicesCount;
      int32_t indicesOffset;

      Enums::Vtx::StripFlags flags;
    };

    struct StripGroup {
      std::vector<Structs::Vtx::Vertex> vertices;
      std::vector<uint16_t> indices;
      std::vector<Strip> strips;

      Enums::Vtx::StripGroupFlags flags;
    };

    struct Mesh {
      std::vector<StripGroup> stripGroups;
      Enums::Vtx::MeshFlags flags;
    };

    struct ModelLod {
      std::vector<Mesh> meshes;
      float switchPoint;
    };

    struct Model {
      std::vector<ModelLod> levelOfDetails;
    };

    struct BodyPart {
      std::vector<Model> models;
    };

    struct MaterialReplacement {
      int16_t replacementId;
      std::string replacementName;
    };

    /**
     * Parses a .vtx file contained in the given buffer into an easier to use and more modern structure.
     * No ownership of the data is taken as all contents are copied into new structs.
     *
     * @param data
     * @param checksum Optional checksum to validate against the header's
     */
    explicit Vtx(
      const std::weak_ptr<std::vector<std::byte>>& data, const std::optional<int32_t>& checksum = std::nullopt
    );

    /**
     * Gets the checksum shared by the MDL, VTX and VVD from the header.
     * @remarks Can be used to loosely verify that a collection of MDL, VTX and VVD files were compiled from the same asset.
     * @return int32_t checksum
     */
    [[nodiscard]] int32_t getChecksum() const;

    [[nodiscard]] const std::vector<MaterialReplacement>& getMaterialReplacements(const int lod) const;
    [[nodiscard]] const std::vector<BodyPart>& getBodyParts() const;

  private:
    Structs::Vtx::Header header;
    std::vector<BodyPart> bodyParts;
    std::vector<std::vector<MaterialReplacement>> materialReplacementsByLod;
  };
}
