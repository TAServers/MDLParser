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
	if (sizeof(Header) + sizeof(Fixup) * header->numFixups + (sizeof(Vector4D) + sizeof(Vertex)) * mNumVertices > dataSize) return;

	mpVertices = reinterpret_cast<Vertex*>(malloc(sizeof(Vertex) * mNumVertices));
	mpTangents = reinterpret_cast<Vector4D*>(malloc(sizeof(Vector4D) * mNumVertices));
	if (mpVertices == nullptr || mpTangents == nullptr) return;

	if (header->numFixups == 0) {
		memcpy(mpTangents, pFileData + header->tangentDataOffset, sizeof(Vector4D) * mNumVertices);
		memcpy(mpVertices, pFileData + header->vertexDataOffset, sizeof(Vertex) * mNumVertices);
	} else {
		size_t offset = 0;
		const Fixup* fixups = reinterpret_cast<const Fixup*>(pFileData + header->fixupTableOffset);

		for (int fixupIdx = 0; fixupIdx < header->numFixups; fixupIdx++) {
			if (fixups[fixupIdx].lod < rootLod) continue;

			memcpy(
				mpTangents + offset,
				reinterpret_cast<const Vertex*>(pFileData + header->tangentDataOffset) + fixups[fixupIdx].sourceVertexId,
				sizeof(Vector4D) * fixups[fixupIdx].numVertices
			);
			memcpy(
				mpVertices + offset,
				reinterpret_cast<const Vertex*>(pFileData + header->vertexDataOffset) + fixups[fixupIdx].sourceVertexId,
				sizeof(Vertex) * fixups[fixupIdx].numVertices
			);

			offset += fixups[fixupIdx].numVertices;
		}
	}

	mIsValid = true;
}

VVD::~VVD()
{
	if (mpVertices != nullptr) free(mpVertices);
	if (mpTangents != nullptr) free(mpTangents);
}

bool VVD::IsValid() const { return mIsValid; }

int32_t VVD::GetNumVertices() const { return mNumVertices; }

const Vertex* VVD::GetVertex(const int i) const
{
	return mpVertices + i;
}
const Vector4D* VVD::GetTangent(const int i) const
{
	return mpTangents + i;
}
