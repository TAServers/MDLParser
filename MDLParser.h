#pragma once

#include <cstdint>

#include "source/Structs.h"

class MDL
{
private:
	bool mIsValid = false;

	uint8_t* mpData;
	size_t mDataSize = 0U;

	const MDLStructs::Header* mpHeader;

	bool mHasHeader2 = false;
	const MDLStructs::Header2* mpHeader2 = nullptr;

	const MDLStructs::Bone* mpBones;
	size_t mNumBones = 0U;

	template<class T>
	const T* ParseSection(const int32_t offset, const int32_t count)
	{
		if (offset < 0 || offset + count * sizeof(T) > mDataSize) return nullptr;
		return reinterpret_cast<const T*>(mpData + offset);
	}

public:
	// Parses a MDL file from raw data
	// You will also need to parse the VVD file for vertex data
	MDL(const uint8_t* pFileData, const size_t dataSize);
	~MDL();

	bool IsValid() const;
};
