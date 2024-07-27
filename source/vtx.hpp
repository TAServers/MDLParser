#pragma once

#include "enums.hpp"
#include "structs/vtx.hpp"
#include <memory>
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

    Vtx(const std::weak_ptr<std::vector<std::byte>>& data, const int32_t checksum);

    [[nodiscard]] const std::vector<MaterialReplacement>& getMaterialReplacements(const int lod) const;
    [[nodiscard]] const std::vector<BodyPart>& getBodyParts() const;

  private:
    Structs::Vtx::Header header;
    std::vector<BodyPart> bodyParts;
    std::vector<std::vector<MaterialReplacement>> materialReplacementsByLod;
  };
}
