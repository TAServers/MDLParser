#pragma once

#include "source/errors.hpp"
#include "source/Structs.h"
#include <optional>

class Vvd {
private:
  std::vector<VVDStructs::Vertex> vertices;
  std::vector<MDLStructs::Vector4D> tangents;

  int32_t numLoDs = 0;

public:
  Vvd(std::vector<std::byte> data, const int32_t checksum);

  [[nodiscard]] const std::vector<VVDStructs::Vertex>& getVertices() const;
  [[nodiscard]] const std::vector<MDLStructs::Vector4D>& getTangents() const;
  [[nodiscard]] int32_t getNumLoDs() const;
};

class Vtx {
private:
  std::vector<std::byte> vtxData;
  VTXStructs::Header header;

public:
  Vtx(std::vector<std::byte> data, const int32_t checksum);

  [[nodiscard]] const std::vector<VTXStructs::MaterialReplacement> getMaterialReplacements(const int lod) const;
  [[nodiscard]] const std::vector<VTXStructs::BodyPart>& getBodyParts() const;
};

class Mdl {
private:
  Vvd vvd;
  Vtx vtx;

  std::vector<std::byte> mdlData;

  MDLStructs::Header header;
  std::optional<MDLStructs::Header2> header2;

public:
  Mdl(std::vector<std::byte> mdlData, std::vector<std::byte> vvdData, std::vector<std::byte> vtxData);
  ~Mdl() = default;

  Mdl(const Mdl&) = delete;
  Mdl& operator=(const Mdl&) = delete;

  Mdl(Mdl&& other) noexcept :
    vvd(std::move(other.vvd)), vtx(std::move(other.vtx)), mdlData(std::move(other.mdlData)), header(other.header),
    header2(other.header2) {}
  Mdl& operator=(Mdl&& other) noexcept {
    vvd = std::move(other.vvd);
    vtx = std::move(other.vtx);
    mdlData = std::move(other.mdlData);

    return *this;
  }

  [[nodiscard]] int32_t getChecksum() const;

  [[nodiscard]] int32_t getNumBodyParts() const;
  void getBodyPart(
    const int i, const MDLStructs::BodyPart** pMDLBodyPartOut, const VTXStructs::BodyPart** pVTXBodyPartOut
  ) const;

  [[nodiscard]] int32_t getNumLoDs() const;
  [[nodiscard]] const std::vector<VVDStructs::Vertex>& getVertices() const;
  [[nodiscard]] const std::vector<MDLStructs::Vector4D>& getTangents() const;

  [[nodiscard]] int32_t getNumMaterials() const;
  [[nodiscard]] int32_t getNumSkinFamilies() const;
  [[nodiscard]] int16_t getMaterialIdx(const int skin, int materialId) const;
  [[nodiscard]] const char* getMaterialName(const int i) const;

  [[nodiscard]] const std::vector<std::string>& getMaterialDirectories() const;

  [[nodiscard]] const std::vector<MDLStructs::Bone>& getBones() const;
};
