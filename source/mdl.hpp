#pragma once

#include "./structs/mdl.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace MdlParser {
  class Mdl {
  public:
    struct Mesh {
      /**
       * Column of the skin lookup table to use for this mesh.
       * @code
       * mdl.getSkinLookupTable()[skinFamily][mesh.material]
       * @endcode
       */
      int32_t material;

      /**
       * Offset to be added to the parent Model's offsets into the VVD's vertex and tangent arrays.
       * @remark Unlike Model, each Mesh has only one offset for both vertices and tangents.
       */
      int32_t vertexOffset;

      /**
       * Number of vertices and tangents in this mesh.
       */
      int32_t vertexCount;
    };

    struct Model {
      std::vector<Mesh> meshes;

      /**
       * Offset into the VVD's vertex array.
       */
      int32_t vertexOffset;

      /**
       * Offset into the VVD's tangent array.
       */
      int32_t tangentsOffset;

      /**
       * Number of vertices and tangents in this model.
       */
      int32_t vertexCount;
    };

    /**
     * A body part (or body group) is a group of models of which exactly one will be displayed at a given time.
     * @remark This is used for toggling GMan's briefcase, selecting the handle type of a door, and much more.
     */
    struct BodyPart {
      /**
       * Human readable name for this body part.
       */
      std::string name;
      std::vector<Model> models;
    };

    struct Bone {
      std::string name;

      int32_t parent;

      Structs::Vector position;
      Structs::Quaternion orientation;
      Structs::RadianEuler orientationEuler;
      Structs::Vector positionScale;
      Structs::Vector orientationScale;

      /**
       * Matrix to translate vertices from model space to bone space for skinning.
       */
      Structs::Matrix3x4 poseToBone;

      int32_t flags;
    };

    struct Texture {
      std::string name;
      int32_t flags;
    };

    /**
     * Parses a .mdl file contained in the given buffer into an easier to use and more modern structure.
     * No ownership of the data is taken as all contents are copied into new structs.
     *
     * @param data
     */
    explicit Mdl(const std::weak_ptr<std::vector<std::byte>>& data);

    [[nodiscard]] int32_t getChecksum() const;

    [[nodiscard]] const std::vector<BodyPart>& getBodyParts() const;

    /**
     * Gets the list of directories (relative to /materials) which contain the textures used by the model.
     * As each texture only includes its filename, you must search through this list of directories to find
     * the actual path to the image asset.
     *
     * @return List of paths relative to /materials
     */
    [[nodiscard]] const std::vector<std::string>& getTextureDirectories() const;
    [[nodiscard]] const std::vector<Texture>& getTextures() const;

    /**
     * Gets the skin lookup table as a row-major 2-dimensional array.
     * To map from a material index to a texture index for a given skin, simply do:
     * @code
     * mdl.getSkinLookupTable()[skinFamily][mesh.material]
     * @endcode
     * @return Skin lookup table as a 2D array
     */
    [[nodiscard]] const std::vector<std::vector<int16_t>>& getSkinLookupTable() const;

    [[nodiscard]] const std::vector<Bone>& getBones() const;

  private:
    Structs::Mdl::Header header;
    std::optional<Structs::Mdl::Header2> header2;

    std::vector<BodyPart> bodyParts;

    std::vector<std::string> textureDirectories;
    std::vector<Texture> textures;
    std::vector<std::vector<int16_t>> skins;

    std::vector<Bone> bones;
  };
}
