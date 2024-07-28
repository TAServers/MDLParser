#include "mdl.hpp"
#include "helpers/offset-data-view.hpp"
#include "structs/vvd.hpp"

namespace MdlParser {
  using Structs::Mdl::Header;
  using Structs::Mdl::Header2;

  namespace {
    constexpr int32_t FILE_ID = u'I' + (u'D' << 8u) + (u'S' << 16u) + (u'T' << 24u);

    Mdl::Mesh parseMesh(const Structs::Mdl::Mesh& mesh) {
      return {
        .material = mesh.material,
        .vertexOffset = mesh.vertsOffset,
        .vertexCount = mesh.vertsCount,
      };
    }

    Mdl::Model parseModel(const OffsetDataView& data, const Structs::Mdl::Model& model) {
      std::vector<Mdl::Mesh> meshes;
      meshes.reserve(model.meshesCount);

      for (const auto& mesh : data.parseStructArrayWithoutOffsets<Structs::Mdl::Mesh>(
             model.meshesOffset, model.meshesCount, "Failed to parse MDL mesh array"
           )) {
        meshes.push_back(parseMesh(mesh));
      }

      return {
        .meshes = std::move(meshes),
        .vertexOffset = model.vertsOffset / static_cast<int32_t>(sizeof(Structs::Vvd::Vertex)),
        .tangentsOffset = model.tangentsOffset / static_cast<int32_t>(sizeof(Structs::Vector4D)),
        .vertexCount = model.vertsCount,
      };
    }

    Mdl::BodyPart parseBodyPart(const OffsetDataView& data, const Structs::Mdl::BodyPart& bodyPart) {
      std::vector<Mdl::Model> models;
      models.reserve(bodyPart.modelsCount);

      for (const auto& [model, offset] : data.parseStructArray<Structs::Mdl::Model>(
             bodyPart.modelsOffset, bodyPart.modelsCount, "Failed to parse MDL model array"
           )) {
        models.push_back(parseModel(data.withOffset(offset), model));
      }

      return {
        .name = data.parseString(bodyPart.szNameIndex, "Failed to parse MDL body part name"),
        .models = std::move(models),
      };
    }

    std::vector<std::string> parseTextureDirectories(const OffsetDataView& data, const Structs::Mdl::Header& header) {
      std::vector<std::string> textureDirectories;
      textureDirectories.reserve(header.textureDirCount);

      for (const auto textureDirectoryOffset : data.parseStructArrayWithoutOffsets<int32_t>(
             header.textureDirOffset, header.textureDirCount, "Failed to parse MDL texture directory list"
           )) {
        textureDirectories.push_back(data.parseString(textureDirectoryOffset, "Failed to parse MDL texture directory"));
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

  Mdl::Mdl(const std::weak_ptr<std::vector<std::byte>>& data, const std::optional<int32_t>& checksum) {
    const OffsetDataView dataView(data);
    header = dataView.parseStruct<Header>(0, "Failed to parse MDL header").first;

    if (header.id != FILE_ID) {
      throw Errors::InvalidHeader("MDL header file ID does not match packed IDST");
    }
    if (header.version > Header::MAX_SUPPORTED_VERSION) {
      throw Errors::InvalidHeader("MDL version is unsupported (greater than 48)");
    }
    if (checksum.has_value() && header.checksum != checksum.value()) {
      throw InvalidChecksum("MDL checksum does not match");
    }

    header2 = header.header2Offset >= sizeof(Header)
      ? std::optional(dataView.parseStruct<Header2>(header.header2Offset, "Failed to parse second MDL header").first)
      : std::nullopt;

    bodyParts.reserve(header.bodypartCount);
    for (const auto& [bodyPart, offset] : dataView.parseStructArray<Structs::Mdl::BodyPart>(
           header.bodypartOffset, header.bodypartCount, "Failed to parse MDL body part array"
         )) {
      bodyParts.push_back(parseBodyPart(dataView.withOffset(offset), bodyPart));
    }

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
