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
)
{
	if (
		pMDLData == nullptr || mdlSize == 0 ||
		pVVDData == nullptr || vvdSize == 0 ||
		pVTXData == nullptr || vtxSize == 0
	) return;

	mpData = static_cast<uint8_t*>(malloc(mdlSize));
	if (mpData == nullptr) return;
	memcpy(mpData, pMDLData, mdlSize);

	mpHeader = reinterpret_cast<const Header*>(mpData);
	if (mpHeader->id != FILE_ID || mpHeader->version > 48) return;

	mpVVD = new (std::nothrow) VVD(pVVDData, vvdSize, mpHeader->checksum);
	if (mpVVD == nullptr || !mpVVD->IsValid()) return;

	mpVTX = new (std::nothrow) VTX(pVTXData, vtxSize, mpHeader->checksum);
	if (mpVTX == nullptr || !mpVTX->IsValid()) return;

	mIsValid = true;
}

MDL::~MDL()
{
	if (mpData != nullptr) free(mpData);
	if (mpVVD != nullptr) delete mpVVD;
	if (mpVTX != nullptr) delete mpVTX;
}

bool MDL::IsValid() const { return mIsValid; }

int32_t MDL::GetChecksum() const { return mpHeader->checksum; }

int32_t MDL::GetNumBodyParts() const { return mpHeader->bodypartCount; }
void MDL::GetBodyPart(
	const int i,
	const BodyPart** pMDLBodyPartOut,
	const VTXStructs::BodyPart** pVTXBodyPartOut
) const
{
	*pMDLBodyPartOut = mpHeader->GetBodyPart(i);
	*pVTXBodyPartOut = mpVTX->GetBodyPart(i);
}

const VVDStructs::Vertex* MDL::GetVertex(const int i) const
{
	return mpVVD->GetVertex(i);
}
const MDLStructs::Vector4D* MDL::GetTangent(const int i) const
{
	return mpVVD->GetTangent(i);
}
