#include "../MDLParser.h"

#include <cstdlib>
#include <cstring>

using namespace VTXStructs;

VTX::VTX(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum)
{
	if (pFileData == nullptr || dataSize == 0 || dataSize < sizeof(Header)) return;

	mpData = static_cast<uint8_t*>(malloc(dataSize));
	if (mpData == nullptr) return;
	memcpy(mpData, pFileData, dataSize);

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
