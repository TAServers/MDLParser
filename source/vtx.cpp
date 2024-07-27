#include "vtx.hpp"
#include "errors.hpp"
#include "helpers/offset-data-view.hpp"
#include "helpers/parse-string.hpp"
#include "helpers/parse-struct.hpp"
#include "structs/vtx.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace MdlParser {
  using Structs::Vtx::Header;
  using namespace Errors;

  namespace {
    Vtx::Strip parseStrip(const Structs::Vtx::Strip& strip) {
      return {
        .verticesCount = strip.numVerts,
        .verticesOffset = strip.vertOffset,
        .indicesCount = strip.numIndices,
        .indicesOffset = strip.indexOffset,
        .flags = strip.flags,
      };
    }

    Vtx::StripGroup parseStripGroup(const OffsetDataView& data, const Structs::Vtx::StripGroup& stripGroup) {
      std::vector<Vtx::Strip> strips;
      strips.reserve(stripGroup.numStrips);

      for (const auto& [strip, _] : data.parseStructArray<Structs::Vtx::Strip>(
             stripGroup.stripOffset, stripGroup.numStrips, "Failed to parse VTX strip array"
           )) {
        checkBounds(
          strip.vertOffset, strip.numVerts, stripGroup.numVerts, "VTX strip accesses outside strip group vertex data"
        );
        checkBounds(
          strip.indexOffset,
          strip.numIndices,
          stripGroup.numIndices,
          "VTX strip accesses outside strip group index data"
        );

        strips.emplace_back(parseStrip(strip));
      }

      return {
        .vertices = data.parseStructArrayWithoutOffsets<Structs::Vtx::Vertex>(
          stripGroup.vertOffset, stripGroup.numVerts, "Failed to parse VTX vertex array"
        ),
        .indices = data.parseStructArrayWithoutOffsets<uint16_t>(
          stripGroup.indexOffset, stripGroup.numIndices, "Failed to parse VTX index array"
        ),
        .strips = std::move(strips),
        .flags = stripGroup.flags,
      };
    }

    Vtx::Mesh parseMesh(const OffsetDataView& data, const Structs::Vtx::Mesh& mesh) {
      std::vector<Vtx::StripGroup> stripGroups;
      stripGroups.reserve(mesh.numStripGroups);

      for (const auto& [stripGroup, offset] : data.parseStructArray<Structs::Vtx::StripGroup>(
             mesh.stripGroupHeaderOffset, mesh.numStripGroups, "Failed to parse VTX strip group array"
           )) {
        stripGroups.emplace_back(parseStripGroup(data.withOffset(offset), stripGroup));
      }

      return {.stripGroups = stripGroups, .flags = mesh.flags};
    }

    Vtx::ModelLod parseModelLod(const OffsetDataView& data, const Structs::Vtx::ModelLoD& lod) {
      std::vector<Vtx::Mesh> meshes;
      meshes.reserve(lod.numMeshes);

      for (const auto& [mesh, offset] :
           data.parseStructArray<Structs::Vtx::Mesh>(lod.meshOffset, lod.numMeshes, "Failed to parse VTX mesh array")) {
        meshes.emplace_back(parseMesh(data.withOffset(offset), mesh));
      }

      return {.meshes = std::move(meshes), .switchPoint = lod.switchPoint};
    }

    Vtx::Model parseModel(const OffsetDataView& data, const Structs::Vtx::Model& model) {
      std::vector<Vtx::ModelLod> lods;
      lods.reserve(model.numLoDs);

      for (const auto& [lod, offset] : data.parseStructArray<Structs::Vtx::ModelLoD>(
             model.lodOffset, model.numLoDs, "Failed to parse VTX model LoD array"
           )) {
        lods.emplace_back(parseModelLod(data.withOffset(offset), lod));
      }

      return {.levelOfDetails = std::move(lods)};
    }

    Vtx::BodyPart parseBodyPart(
      const OffsetDataView& data, const Structs::Vtx::BodyPart& bodyPart, int32_t expectedLods
    ) {
      std::vector<Vtx::Model> models;
      models.reserve(bodyPart.numModels);

      for (const auto& [model, offset] : data.parseStructArray<Structs::Vtx::Model>(
             bodyPart.modelOffset, bodyPart.numModels, "Failed to parse VTX model array"
           )) {
        if (model.numLoDs != expectedLods) {
          throw InvalidBody("VTX model LoD count does not match header");
        }

        models.emplace_back(parseModel(data.withOffset(offset), model));
      }

      return {.models = std::move(models)};
    }
  }

  Vtx::Vtx(const std::shared_ptr<std::vector<std::byte>>& data, const int32_t checksum) {
    const OffsetDataView dataView(data);
    header = dataView.parseStruct<Header>(0, "Failed to parse VTX header").first;

    if (header.version != Header::SUPPORTED_VERSION) {
      throw UnsupportedVersion("VTX version is unsupported");
    }
    if (header.checksum != checksum) {
      throw InvalidChecksum("VTX checksum does not match MDL's");
    }

    bodyParts.reserve(header.numBodyParts);
    for (const auto& [bodyPart, offset] : dataView.parseStructArray<Structs::Vtx::BodyPart>(
           header.bodyPartOffset, header.numBodyParts, "Failed to parse VTX body part array"
         )) {
      bodyParts.emplace_back(parseBodyPart(dataView.withOffset(offset), bodyPart, header.numLoDs));
    }

    materialReplacementsByLod.reserve(header.numLoDs);
    for (const auto& [replacementList, replacementListOffset] :
         dataView.parseStructArray<Structs::Vtx::MaterialReplacementList>(
           header.materialReplacementListOffset, header.numLoDs, "Failed to parse VTX material replacement lists"
         )) {
      std::vector<Vtx::MaterialReplacement> replacements;
      replacements.reserve(replacementList.replacementCount);

      for (const auto& [replacement, replacementOffset] : dataView.withOffset(replacementListOffset)
                                                            .parseStructArray<Structs::Vtx::MaterialReplacement>(
                                                              replacementList.replacementOffset,
                                                              replacementList.replacementCount,
                                                              "Failed to parse VTX material replacements"
                                                            )) {
        replacements.push_back({
          .replacementId = replacement.materialId,
          .replacementName =
            dataView.withOffset(replacementOffset)
              .parseString(replacement.replacementMaterialNameOffset, "Failed to parse VTX material replacement name"),
        });
      }

      materialReplacementsByLod.emplace_back(std::move(replacements));
    }
  }

  const std::vector<Vtx::MaterialReplacement>& Vtx::getMaterialReplacements(const int lod) const {
    checkBounds(lod, 1, materialReplacementsByLod.size(), "Level of detail is outside range");
    return materialReplacementsByLod[lod];
  }

  const std::vector<Vtx::BodyPart>& Vtx::getBodyParts() const {
    return bodyParts;
  }
}
