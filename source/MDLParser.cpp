#include "../MDLParser.h"

#include <cstdlib>
#include <cstring>
#include <cstdint>

using namespace MDLStructs;

constexpr int32_t FILE_ID = 'I' + ('D' << 8) + ('S' << 16) + ('T' << 24);

MDL::MDL(const uint8_t* pFileData, const size_t dataSize) : mDataSize(dataSize)
{
	if (pFileData == nullptr || dataSize == 0) return;

	mpData = static_cast<uint8_t*>(malloc(dataSize));
	if (mpData == nullptr) return;
	memcpy(mpData, pFileData, dataSize);

	mpHeader = ParseSection<Header>(0, 1);
	if (mpHeader == nullptr || mpHeader->id != FILE_ID) return;

	mIsValid = true;
}

MDL::~MDL()
{
	if (mpData != nullptr) {
		free(mpData);
		mpData = nullptr;
	}
}

bool MDL::IsValid() const
{
	return mIsValid;
}
