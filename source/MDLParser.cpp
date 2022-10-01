#include "../MDLParser.h"

#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <new>

using namespace MDLStructs;

constexpr int32_t FILE_ID = 'I' + ('D' << 8) + ('S' << 16) + ('T' << 24);

MDL::MDL(
	const uint8_t* pMDLData, const size_t mdlSize,
	const uint8_t* pVVDData, const size_t vvdSize,
	const uint8_t* pVTXData, const size_t vtxSize
) : mDataSize(mdlSize)
{
	if (
		pMDLData == nullptr || mdlSize == 0 ||
		pVVDData == nullptr || vvdSize == 0
		//pVTXData == nullptr || vtxSize == 0
	) return;

	mpData = static_cast<uint8_t*>(malloc(mDataSize));
	if (mpData == nullptr) return;
	memcpy(mpData, pMDLData, mDataSize);

	mpHeader = reinterpret_cast<const Header*>(mpData);
	if (mpHeader->id != FILE_ID || mpHeader->version > 48) return;

	mVVD = new (std::nothrow) VVD(pVVDData, vvdSize, mpHeader->checksum);
	if (mVVD == nullptr || !mVVD->IsValid()) return;

	mVTX = new (std::nothrow) VTX();
	//if (mVTX == nullptr || !mVTX.IsValid()) return;

	mIsValid = true;
}

MDL::~MDL()
{
	if (mpData != nullptr) free(mpData);
	if (mVVD != nullptr) delete mVVD;
	if (mVTX != nullptr) delete mVTX;
}

bool MDL::IsValid() const { return mIsValid; }

int32_t MDL::GetChecksum() const { return mpHeader->checksum; }
