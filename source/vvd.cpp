#include "vvd.hpp"
#include "helpers/parse-struct.hpp"
#include <cstdint>

namespace MdlParser {
  using Structs::Vector4D;
  using namespace Structs::Vvd;
  using namespace Errors;
  constexpr int32_t FILE_ID = u'I' + (u'D' << 8u) + (u'S' << 16u) + (u'V' << 24u);

  Vvd::Vvd(std::vector<std::byte>& data, const int32_t checksum) {
    const int32_t rootLod = 0;

    const auto header = parseStruct<Header>(data, 0, "Failed to parse VVD header");

    if (header.id != FILE_ID) {
      throw InvalidHeader("VVD header ID does not match IDSV");
    }
    if (header.version != Header::SUPPORTED_VERSION) {
      throw UnsupportedVersion("VVD version is unsupported");
    }
    if (header.checksum != checksum) {
      throw InvalidChecksum("VVD checksum does not match MDL's");
    }

    const auto numVertices = header.numLoDVertices[rootLod];
    if (sizeof(Header) + sizeof(Fixup) * header.numFixups + (sizeof(Vector4D) + sizeof(Vertex)) * numVertices >
        data.size()) {
      throw InvalidDataSize("Size of VVD with given number of vertices exceeds data size");
    }

    numLods = header.numLoDs;

    if (header.numFixups == 0) {
      vertices = parseStructArray<Vertex>(data, header.vertexDataOffset, numVertices, "Failed to parse VVD vertices");
      tangents =
        parseStructArray<Vector4D>(data, header.tangentDataOffset, numVertices, "Failed to parse VVD tangents");
    } else {
      const auto fixups =
        parseStructArray<Fixup>(data, header.fixupTableOffset, header.numFixups, "Failed to parse VVD fixups");
      const auto originalVertices =
        parseStructArray<Vertex>(data, header.vertexDataOffset, numVertices, "Failed to parse VVD vertices");
      const auto originalTangents =
        parseStructArray<Vector4D>(data, header.tangentDataOffset, numVertices, "Failed to parse VVD tangents");

      vertices.reserve(numVertices);
      tangents.reserve(numVertices);

      for (const auto& fixup : fixups) {
        if (fixup.lod < rootLod || fixup.numVertices <= 0 || fixup.sourceVertexId < 0) {
          continue;
        }

        if (fixup.sourceVertexId + fixup.numVertices > numVertices) {
          throw OutOfBoundsAccess("VVD fixup accesses outside vertex data");
        }

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
