#include "vvd.hpp"
#include "helpers/offset-data-view.hpp"
#include <cstdint>
#include <memory>
#include <optional>

namespace MdlParser {
  using Structs::Vector4D;
  using namespace Structs::Vvd;
  using namespace Errors;

  namespace {
    constexpr int32_t FILE_ID = u'I' + (u'D' << 8u) + (u'S' << 16u) + (u'V' << 24u);
  }

  Vvd::Vvd(const std::weak_ptr<std::vector<std::byte>>& data, const std::optional<int32_t>& checksum) {
    const OffsetDataView dataView(data);
    const int32_t rootLod = 0;

    header = dataView.parseStruct<Header>(0, "Failed to parse VVD header").first;

    if (header.id != FILE_ID) {
      throw InvalidHeader("VVD header ID does not match IDSV");
    }
    if (header.version != Header::SUPPORTED_VERSION) {
      throw UnsupportedVersion("VVD version is unsupported");
    }
    if (checksum.has_value() && header.checksum != checksum.value()) {
      throw InvalidChecksum("VVD checksum does not match");
    }

    const auto numVertices = header.numLoDVertices[rootLod];
    if (sizeof(Header) + sizeof(Fixup) * header.numFixups + (sizeof(Vector4D) + sizeof(Vertex)) * numVertices >
        data.lock()->size()) {
      throw InvalidBody("Size of VVD with given number of vertices exceeds data size");
    }

    numLods = header.numLoDs;

    if (header.numFixups == 0) {
      vertices = dataView.parseStructArrayWithoutOffsets<Vertex>(
        header.vertexDataOffset, numVertices, "Failed to parse VVD vertices"
      );
      tangents = dataView.parseStructArrayWithoutOffsets<Vector4D>(
        header.tangentDataOffset, numVertices, "Failed to parse VVD tangents"
      );
    } else {
      const auto fixups = dataView.parseStructArrayWithoutOffsets<Fixup>(
        header.fixupTableOffset, header.numFixups, "Failed to parse VVD fixups"
      );
      const auto originalVertices = dataView.parseStructArrayWithoutOffsets<Vertex>(
        header.vertexDataOffset, numVertices, "Failed to parse VVD vertices"
      );
      const auto originalTangents = dataView.parseStructArrayWithoutOffsets<Vector4D>(
        header.tangentDataOffset, numVertices, "Failed to parse VVD tangents"
      );

      vertices.reserve(numVertices);
      tangents.reserve(numVertices);

      for (const auto& fixup : fixups) {
        if (fixup.lod < rootLod || fixup.numVertices <= 0 || fixup.sourceVertexId < 0) {
          continue;
        }

        checkBounds(fixup.sourceVertexId, fixup.numVertices, numVertices, "VVD fixup accesses outside vertex data");

        vertices.insert(
          vertices.end(),
          originalVertices.begin() + fixup.sourceVertexId,
          originalVertices.begin() + fixup.sourceVertexId + fixup.numVertices
        );

        tangents.insert(
          tangents.end(),
          originalTangents.begin() + fixup.sourceVertexId,
          originalTangents.begin() + fixup.sourceVertexId + fixup.numVertices
        );
      }
    }
  }

  int32_t Vvd::getChecksum() const {
    return header.checksum;
  }

  const std::vector<Vertex>& Vvd::getVertices() const {
    return vertices;
  }
  const std::vector<Vector4D>& Vvd::getTangents() const {
    return tangents;
  }

  int32_t Vvd::getNumLods() const {
    return numLods;
  }
}
