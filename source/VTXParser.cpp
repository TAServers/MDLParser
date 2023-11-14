#include "../MDLParser.h"

#include <cstring>

#include "Ints.h"

using namespace VTXStructs;

void VTX::CopyFrom(const VTX& src)
{
	mIsValid = src.mIsValid;

	if (mIsValid) {
		mDataSize = src.mDataSize;
		mpData = static_cast<uint8_t*>(malloc(mDataSize));
		memcpy(mpData, src.mpData, mDataSize);

		mpHeader = reinterpret_cast<const Header*>(mpData);
	}
}
VTX::VTX(const VTX& src)
{
	CopyFrom(src);
}
VTX& VTX::operator=(const VTX& src)
{
	CopyFrom(src);
	return *this;
}

VTX::VTX(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum) : mDataSize(dataSize)
{
	if (pFileData == nullptr || mDataSize == 0 || mDataSize < sizeof(Header)) return;

	mpData = static_cast<uint8_t*>(malloc(mDataSize));
	if (mpData == nullptr) return;
	memcpy(mpData, pFileData, mDataSize);

	mpHeader = reinterpret_cast<const Header*>(mpData);
	if (
		mpHeader->version != Header::SUPPORTED_VERSION ||
		mpHeader->checksum != checksum
	) return;

	for (int i = 0; i < mpHeader->numBodyParts; i++) {
		const BodyPart* bodypart = mpHeader->GetBodyPart(i);
		for (int j = 0; j < bodypart->numModels; j++) {
			if (bodypart->GetModel(j)->numLoDs != mpHeader->numLoDs) return; // number of LoDs should match the header
		}
	}

	mIsValid = true;
}

VTX::~VTX()
{
	if (mpData != nullptr) free(mpData);
}

bool VTX::IsValid() const { return mIsValid; }

const VTXStructs::MaterialReplacementList* VTX::GetMaterialReplacementList(const int lod) const
{
	return mpHeader->GetMaterialReplacementList(lod);
}

int32_t VTX::GetNumBodyParts() const
{
	return mpHeader->numBodyParts;
}
const VTXStructs::BodyPart* VTX::GetBodyPart(const int i) const
{
	return mpHeader->GetBodyPart(i);
}
