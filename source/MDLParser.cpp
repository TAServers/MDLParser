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

int32_t MDL::GetNumVertices() const
{
	return mpVVD->GetNumVertices();
}
const VVDStructs::Vertex* MDL::GetVertex(const int i) const
{
	return mpVVD->GetVertex(i);
}
const MDLStructs::Vector4D* MDL::GetTangent(const int i) const
{
	return mpVVD->GetTangent(i);
}

int32_t MDL::GetNumMaterials() const
{
	return mpHeader->textureCount;
}
int32_t MDL::GetNumSkinFamilies() const
{
	return mpHeader->skinFamilyCount;
}

int16_t MDL::GetMaterialIdx(const int skin, int materialId) const
{
	// The VTX file has "material replacement" lists, however I can't find a single usage of them in the engine
	// 
	// I'm not sure if they're meant to be indexed with the material id,
	// or if you loop over them and compare the desired id with the id in the struct
	// 
	// If they are used then it's in one of the closed source areas of the engine

	return mpHeader->GetSkin(skin, materialId);
}
const char* MDL::GetMaterial(const int i) const
{
	return mpHeader->GetTexture(i)->GetName();
}

int32_t MDL::GetNumBones() const
{
	return mpHeader->boneCount;
}
const Bone* MDL::GetBone(const int i) const
{
	return mpHeader->GetBone(i);
}
