#pragma once

#include <cstdint>

#include "source/Structs.h"

class VVD;
class VTX;

class MDL
{
private:
	bool mIsValid = false;

	VVD* mpVVD = nullptr;
	VTX* mpVTX = nullptr;

	uint8_t* mpData = nullptr;

	const MDLStructs::Header* mpHeader;

	bool mHasHeader2 = false;
	const MDLStructs::Header2* mpHeader2 = nullptr;

public:
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

	const VVDStructs::Vertex* GetVertex(const int i) const;
	const MDLStructs::Vector4D* GetTangent(const int i) const;

	const char* GetMaterial(const int skin, int materialId) const;
};

class VVD
{
private:
	bool mIsValid = false;

	int32_t mNumVertices = 0U;
	VVDStructs::Vertex*   mpVertices = nullptr;
	MDLStructs::Vector4D* mpTangents = nullptr;

public:
	VVD() {}
	VVD(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum);

	~VVD();

	bool IsValid() const;

	int32_t GetNumVertices() const;

	const VVDStructs::Vertex* GetVertex(const int i) const;
	const MDLStructs::Vector4D* GetTangent(const int i) const;
};

class VTX
{
private:
	bool mIsValid = false;

	uint8_t* mpData = nullptr;
	const VTXStructs::Header* mpHeader;

public:
	VTX() {}
	VTX(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum);

	~VTX();

	bool IsValid() const;

	const VTXStructs::MaterialReplacementList* GetMaterialReplacementList(const int lod) const;

	int32_t GetNumBodyParts() const;
	const VTXStructs::BodyPart* GetBodyPart(const int i) const;
};
