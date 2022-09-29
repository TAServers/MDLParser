#pragma once

#include <cstdint>

#include "source/Structs.h"

class VVD;
class VTX;

class MDL
{
private:
	bool mIsValid = false;

	VVD* mVVD = nullptr;
	VTX* mVTX = nullptr;

	uint8_t* mpData = nullptr;
	size_t mDataSize = 0U;

	const MDLStructs::Header* mpHeader;

	bool mHasHeader2 = false;
	const MDLStructs::Header2* mpHeader2 = nullptr;

	const MDLStructs::Bone* mpBones;

	template<class T>
	const T* ParseSection(const int32_t offset, const int32_t count)
	{
		if (offset < 0 || offset + count * sizeof(T) > mDataSize) return nullptr;
		return reinterpret_cast<const T*>(mpData + offset);
	}

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
	MDLStructs::Vector*     mpPositions = nullptr;
	MDLStructs::Vector*     mpNormals   = nullptr;
	MDLStructs::Vector4D*   mpTangents  = nullptr;
	MDLStructs::Vector2D*   mpUVs       = nullptr;
	MDLStructs::Vector*     mpWeights   = nullptr;
	MDLStructs::ByteVector* mpBones     = nullptr;

public:
	VVD() {}
	VVD(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum);

	~VVD();

	bool IsValid() const;

	size_t GetNumVertices() const;

	const MDLStructs::Vector* GetPositions() const;

	const MDLStructs::Vector* GetNormals() const;
	const MDLStructs::Vector4D* GetTangents() const;

	const MDLStructs::Vector2D* GetUVs() const;

	const MDLStructs::Vector* GetWeights() const;
	const MDLStructs::ByteVector* GetBoneIDs() const;
};

class VTX
{
private:
	bool mIsValid = false;

public:
	VTX() {}
};
