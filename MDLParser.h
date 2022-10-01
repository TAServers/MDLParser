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
};

class VVD
{
private:
	bool mIsValid = false;

	size_t mNumVertices = 0U;
	VVDStructs::Vertex*   mpVertices = nullptr;
	MDLStructs::Vector4D* mpTangents = nullptr;

public:
	VVD() {}
	VVD(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum);

	~VVD();

	bool IsValid() const;

	size_t GetNumVertices() const;

	const VVDStructs::Vertex* GetVertices() const;
	const MDLStructs::Vector4D* GetTangents() const;
};

class VTX
{
private:
	bool mIsValid = false;

	uint8_t* mpData = nullptr;
	const VTXStructs::Header* mpHeader;

public:
	VTX() {}
	VTX(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum, const int32_t numLoDs);

	~VTX();

	bool IsValid() const;

	const VTXStructs::MaterialReplacementListHeader* GetMaterialReplacementList(const int i) const;

	int32_t GetNumBodyParts() const;
	const VTXStructs::BodyPartHeader* GetBodyPart(const int i) const;
};
