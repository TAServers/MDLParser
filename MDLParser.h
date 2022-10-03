#pragma once

#include <cstdint>
#include <string>

#include "source/Structs.h"

class VVD
{
private:
	bool mIsValid = false;

	int32_t mNumVertices = 0U;
	VVDStructs::Vertex*   mpVertices = nullptr;
	MDLStructs::Vector4D* mpTangents = nullptr;

	int32_t mNumLoDs = 0;

	void CopyFrom(const VVD& src);

public:
	VVD() {}
	VVD(const VVD& src);
	VVD& operator=(const VVD& src);

	VVD(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum);
	~VVD();

	bool IsValid() const;

	int32_t GetNumVertices() const;

	const VVDStructs::Vertex* GetVertex(const int i) const;
	const MDLStructs::Vector4D* GetTangent(const int i) const;

	int32_t GetNumLoDs() const;
};

class VTX
{
private:
	bool mIsValid = false;

	size_t mDataSize = 0U;
	uint8_t* mpData = nullptr;

	const VTXStructs::Header* mpHeader = nullptr;

	void CopyFrom(const VTX& src);

public:
	VTX() {}
	VTX(const VTX& src);
	VTX& operator=(const VTX& src);

	VTX(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum);
	~VTX();

	bool IsValid() const;

	const VTXStructs::MaterialReplacementList* GetMaterialReplacementList(const int lod) const;

	int32_t GetNumBodyParts() const;
	const VTXStructs::BodyPart* GetBodyPart(const int i) const;
};

class MDL
{
private:
	bool mIsValid = false;

	VVD mVVD;
	VTX mVTX;

	size_t mDataSize = 0U;
	uint8_t* mpData = nullptr;

	const MDLStructs::Header* mpHeader = nullptr;

	bool mHasHeader2 = false;
	const MDLStructs::Header2* mpHeader2 = nullptr;

	void CopyFrom(const MDL& src);

public:
	MDL() {}
	MDL(const MDL& src);
	MDL& operator=(const MDL& src);

	// Parses a MDL file from raw data, along with VVD and VTX
	MDL(
		const uint8_t* pMDLData, const size_t mdlSize,
		const uint8_t* pVVDData, const size_t vvdSize,
		const uint8_t* pVTXData, const size_t vtxSize
	);
	~MDL();

	bool IsValid() const;

	int32_t GetChecksum() const;

	int32_t GetNumBodyParts() const;
	void GetBodyPart(
		const int i,
		const MDLStructs::BodyPart** pMDLBodyPartOut,
		const VTXStructs::BodyPart** pVTXBodyPartOut
	) const;

	int32_t GetNumLoDs() const;

	int32_t GetNumVertices() const;
	const VVDStructs::Vertex* GetVertex(const int i) const;
	const MDLStructs::Vector4D* GetTangent(const int i) const;

	int32_t GetNumMaterials() const;
	int32_t GetNumSkinFamilies() const;
	int16_t GetMaterialIdx(const int skin, int materialId) const;
	std::string GetMaterial(const int i) const;

	int32_t GetNumBones() const;
	const MDLStructs::Bone* GetBone(const int i) const;
};
