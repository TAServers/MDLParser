#include "mdl.hpp"
#include "helpers/offset-data-view.hpp"

namespace MdlParser {
  using Structs::Mdl::Header;
  using Structs::Mdl::Header2;

  namespace {
    constexpr int32_t FILE_ID = u'I' + (u'D' << 8u) + (u'S' << 16u) + (u'T' << 24u);

    std::vector<Mdl::BodyPart> parseBodyPart(const OffsetDataView& data, const Structs::Mdl::BodyPart& bodyPart) {}

    std::vector<std::string> parseTextureDirectories(const OffsetDataView& data, const Structs::Mdl::Header& header) {
      std::vector<std::string> textureDirectories;
      textureDirectories.reserve(header.textureDirCount);

      for (const auto textureDirectoryOffset : data.parseStructArrayWithoutOffsets<int32_t>(
             header.textureDirOffset, header.textureDirCount, "Failed to parse MDL texture directory list"
           )) {
        textureDirectories.emplace_back(
          data.parseString(textureDirectoryOffset, "Failed to parse MDL texture directory")
        );
      }

      return std::move(textureDirectories);
    }

    std::vector<Mdl::Texture> parseTextures(const OffsetDataView& data, const Structs::Mdl::Header& header) {
      std::vector<Mdl::Texture> textures;
      textures.reserve(header.textureCount);

      for (const auto& [texture, offset] : data.parseStructArray<Structs::Mdl::Texture>(
             header.textureOffset, header.textureCount, "Failed to parse MDL texture array"
           )) {
        textures.push_back({
          .name = data.withOffset(offset).parseString(texture.szNameIndex, "Failed to parse MDL texture name"),
          .flags = texture.flags,
        });
      }

      return std::move(textures);
    }

    std::vector<std::vector<int16_t>> parseSkinTable(const OffsetDataView& data, const Structs::Mdl::Header& header) {
      std::vector<std::vector<int16_t>> skins;
      skins.reserve(header.skinFamilyCount);

      for (size_t family = 0; family < header.skinFamilyCount; family++) {
        skins.push_back(
          data.withOffset(header.skinRefOffset)
            .parseStructArrayWithoutOffsets<int16_t>(
              family * header.skinRefCount * sizeof(int16_t), header.skinRefCount, "Failed to parse MDL skin table row"
            )
        );
      }

      return std::move(skins);
    }

    std::vector<Mdl::Bone> parseBones(const OffsetDataView& data, const Structs::Mdl::Header& header) {
      std::vector<Mdl::Bone> bones;
      bones.reserve(header.boneCount);

      for (const auto& [bone, offset] : data.parseStructArray<Structs::Mdl::Bone>(
             header.boneOffset, header.boneCount, "Failed to parse MDL bone array"
           )) {
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

    textureDirectories = parseTextureDirectories(dataView, header);
    textures = parseTextures(dataView, header);
    skins = parseSkinTable(dataView, header);
    bones = parseBones(dataView, header);
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

  const std::vector<std::vector<int16_t>>& Mdl::getSkinLookupTable() const {
    return skins;
  }

  const std::vector<Mdl::Bone>& Mdl::getBones() const {
    return bones;
  }
}
