#include "../MDLParser.h"

#include <cstdlib>
#include <cstring>

using namespace VTXStructs;

VTX::VTX(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum, const int32_t numLoDs)
{
	if (pFileData == nullptr || dataSize == 0 || dataSize < sizeof(Header)) return;

	mpData = static_cast<uint8_t*>(malloc(dataSize));
	if (mpData == nullptr) return;
	memcpy(mpData, pFileData, dataSize);

	mpHeader = reinterpret_cast<const Header*>(mpData);
	if (
		mpHeader->version != Header::SUPPORTED_VERSION ||
		mpHeader->checksum != checksum ||
		mpHeader->numLoDs != numLoDs // number of LoDs should match the MDL
	) return;

	for (int i = 0; i < mpHeader->numBodyParts; i++) {
		const BodyPartHeader* bodypart = mpHeader->GetBodyPartHeader(i);
		for (int j = 0; j < bodypart->numModels; j++) {
			if (bodypart->GetModelHeader(j)->numLoDs != numLoDs) return; // number of LoDs should match the MDL
		}
	}

	mIsValid = true;
}

VTX::~VTX()
{
	if (mpData != nullptr) free(mpData);
}

bool VTX::IsValid() const { return mIsValid; }

const VTXStructs::MaterialReplacementListHeader* VTX::GetMaterialReplacementList(const int i) const
{
	return mpHeader->GetMaterialReplacementListHeader(i);
}

int32_t VTX::GetNumBodyParts() const
{
	return mpHeader->numBodyParts;
}
const VTXStructs::BodyPartHeader* VTX::GetBodyPart(const int i) const
{
	return mpHeader->GetBodyPartHeader(i);
}
