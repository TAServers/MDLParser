#include "../MDLParser.h"

#include <cstdlib>
#include <cstring>

using namespace VVDStructs;

constexpr int32_t FILE_ID = 'I' + ('D' << 8) + ('S' << 16) + ('V' << 24);

VVD::VVD(const uint8_t* pFileData, const size_t dataSize, const int32_t checksum)
{
	int32_t rootLod = 0;
	if (pFileData == nullptr || dataSize == 0 || dataSize < sizeof(Header)) return;

	const Header* header = reinterpret_cast<const Header*>(pFileData);
	if (
		header->id != FILE_ID ||
		header->version != Header::SUPPORTED_VERSION ||
		header->checksum != checksum
	) return;

	mNumVertices = header->numLoDVertices[rootLod];
	if (sizeof(Header) + sizeof(Fixup) * header->numFixups + (sizeof(Vector4D) + sizeof(Vertex)) * numVertices > dataSize) return;

	mpPositions = malloc(sizeof(Vector)     * numVertices);
	mpNormals   = malloc(sizeof(Vector)     * numVertices);
	mpTangents  = malloc(sizeof(Vector4D)   * numVertices);
	mpUVs       = malloc(sizeof(Vector2D)   * numVertices);
	mpWeights   = malloc(sizeof(Vector)     * numVertices);
	mpBones     = malloc(sizeof(ByteVector) * numVertices);

	if (
		mpPositions == nullptr ||
		mpNormals   == nullptr ||
		mpTangents  == nullptr ||
		mpUVs       == nullptr ||
		mpWeights   == nullptr ||
		mpBones     == nullptr
	) return;

	const Vertex* vertices = reinterpret_cast<const Vertex*>(pFileData + header->vertexDataOffset);

	if (header->numFixups == 0) {
		memcpy(mpTangents, pFileData + header->tangentDataOffset, sizeof(Vector4D) * numVertices);

		for (int vertIdx = 0; vertIdx < mNumVertices; vertIdx++) {
			mpPositions[vertIdx] = vertices[vertIdx].pos;
			mpNormals[vertIdx]   = vertices[vertIdx].normal;
			mpUVs[vertIdx]       = vertices[vertIdx].texCoord;

			mpWeights[vertIdx] = {
				vertices[vertIdx].boneWeights.weight[0],
				vertices[vertIdx].boneWeights.numBones > 1 ? vertices[vertIdx].boneWeights.weight[1] : 0.f,
				vertices[vertIdx].boneWeights.numBones > 2 ? vertices[vertIdx].boneWeights.weight[2] : 0.f
			};
			mpBones[vertIdx] = {
				vertices[vertIdx].boneWeights.bone[0],
				vertices[vertIdx].boneWeights.numBones > 1 ? vertices[vertIdx].boneWeights.bone[1] : 0,
				vertices[vertIdx].boneWeights.numBones > 2 ? vertices[vertIdx].boneWeights.bone[2] : 0
			};
		}
	} else {
		size_t offset = 0;
		const Fixup* fixups = reinterpret_cast<const Fixup*>(pFileData + header->fixupTableOffset);

		for (int fixupIdx = 0; fixupIdx < header->numFixups; fixupIdx++) {
			if (fixups[fixupIdx].lod < rootLod) continue;

			memcpy(
				mpTangents + offset,
				pFileData + header->tangentDataOffset + fixups[fixupIdx].sourceVertexId,
				sizeof(Vector4D) * fixups[fixupIdx].numVertices
			);

			for (int vertIdx = 0; vertIdx < fixups[fixupIdx].numVertices; vertIdx++) {
				int vertIdxSrc = fixups[fixupIdx].sourceVertexId + vertIdx;
				int vertIdxDst = offset + vertIdx;

				mpPositions[vertIdxDst] = vertices[vertIdxSrc].pos;
				mpNormals[vertIdxDst]   = vertices[vertIdxSrc].normal;
				mpUVs[vertIdxDst]       = vertices[vertIdxSrc].texCoord;

				mpWeights[vertIdxDst] = {
					vertices[vertIdxSrc].boneWeights.weight[0],
					vertices[vertIdxSrc].boneWeights.numBones > 1 ? vertices[vertIdxSrc].boneWeights.weight[1] : 0.f,
					vertices[vertIdxSrc].boneWeights.numBones > 2 ? vertices[vertIdxSrc].boneWeights.weight[2] : 0.f
				};
				mpBones[vertIdxDst] = {
					vertices[vertIdxSrc].boneWeights.bone[0],
					vertices[vertIdxSrc].boneWeights.numBones > 1 ? vertices[vertIdxSrc].boneWeights.bone[1] : 0,
					vertices[vertIdxSrc].boneWeights.numBones > 2 ? vertices[vertIdxSrc].boneWeights.bone[2] : 0
				};
			}

			offset += fixups[fixupIdx].numVertices;
		}
	}

	mIsValid = true;
}

VVD::~VVD()
{
	if (mpPositions != nullptr) free(mpPositions);
	if (mpNormals   != nullptr) free(mpNormals);
	if (mpTangents  != nullptr) free(mpTangents);
	if (mpUVs       != nullptr) free(mpUVs);
	if (mpWeights   != nullptr) free(mpWeights);
	if (mpBones     != nullptr) free(mpBones);
}

bool VVD::IsValid() const { return mIsValid; }

size_t VVD::GetNumVertices() const { return mNumVertices; }

const Vector* VVD::GetPositions() const { return mpPositions; }

const Vector* VVD::GetNormals() const { return mpNormals; }
const Vector4D* VVD::GetTangents() const { return mpTangents; }

const Vector2D* VVD::GetUVs() const { return mpUVs; }

const Vector* VVD::GetWeights() const { return mpWeights; }
const ByteVector* VVD::GetBoneIDs() const { return mpBones; }
