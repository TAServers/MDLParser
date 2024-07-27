#include "mdl.hpp"
#include "helpers/offset-data-view.hpp"

namespace MdlParser {
  using Structs::Mdl::Header;
  using Structs::Mdl::Header2;

  namespace {
    constexpr int32_t FILE_ID = u'I' + (u'D' << 8u) + (u'S' << 16u) + (u'T' << 24u);

    std::vector<Mdl::Bone> parseBones(const OffsetDataView& data, size_t boneOffset, size_t boneCount) {
      std::vector<Mdl::Bone> bones;
      bones.reserve(boneCount);

      for (const auto& [bone, offset] :
           data.parseStructArray<Structs::Mdl::Bone>(boneOffset, boneCount, "Failed to parse MDL bone array")) {
        bones.push_back({
          .name = data.withOffset(offset).parseString(bone.szNameIndex, "Failed to parse MDL bone name"),
        });
      }

      return std::move(bones);
    }
  }

  Mdl::Mdl(const std::weak_ptr<std::vector<std::byte>>& data) {
    const OffsetDataView dataView(data);
    header = dataView.parseStruct<Header>(0, "Failed to parse MDL header").first;

    if (header.id != FILE_ID) {
      throw Errors::InvalidHeader("MDL header file ID does not match packed IDST");
    }

    if (header.version > Header::MAX_SUPPORTED_VERSION) {
      throw Errors::InvalidHeader("MDL version is unsupported (greater than 48)");
    }

    header2 = header.header2Offset >= sizeof(Header)
      ? std::optional(dataView.parseStruct<Header2>(header.header2Offset, "Failed to parse second MDL header").first)
      : std::nullopt;

    bones = parseBones(dataView, header.boneOffset, header.boneCount);
  }

  int32_t Mdl::getChecksum() const {
    return header.checksum;
  }

  const std::vector<Mdl::BodyPart>& Mdl::getBodyParts() const {
    return bodyParts;
  }

  const std::vector<std::string>& Mdl::getTextureDirectories() const {
    return textureDirectories;
  }

  const std::vector<Mdl::Texture>& Mdl::getTextures() const {
    return textures;
  }

  const std::vector<std::vector<size_t>> Mdl::getSkinLookupTable() const {
    return skins;
  }

  const std::vector<Mdl::Bone> Mdl::getBones() const {
    return bones;
  }
}
