#pragma once

#include "Enums.h"
#include "Limits.h"

#include <cstdint>

namespace MDLStructs
{
	struct ByteVector
	{
		int8_t x, y, z;
	};

	struct Vector2D
	{
		float x, y;
	};

	struct Vector
	{
		float x, y, z;
	};

	struct Vector4D
	{
		float x, y, z, w;
	};

	typedef Vector4D Quaternion;
	typedef Vector RadianEuler;

	struct Matrix3x4
	{
		float m[3][4];

		float* operator[](int i) { return m[i]; }
		const float* operator[](int i) const { return m[i]; }
	};

	struct Header
	{
		static const int32_t SUPPORTED_VERSION = 48;

		int32_t id; // Model format ID (IDST)
		int32_t version;
		int32_t checksum;
		uint8_t name[64];
		int32_t dataLength; // Size of the file

		Vector eyePosition;
		Vector illumPosition;

		Vector hullMin;
		Vector hullMax;
		Vector viewMin; // Valve dev wiki doesn't know what this is, but likely the render bounds
		Vector viewMax;

		MDLEnums::Flags flags;

		int32_t boneCount;
		int32_t boneOffset;

		int32_t boneControllerCount;
		int32_t boneControllerOffset;

		int32_t hitboxCount;
		int32_t hitboxOffset;

		int32_t localAnimCount;
		int32_t localAnimOffset;

		int32_t localSequenceCount;
		int32_t localSequenceOffset;

		int32_t activityListVersion;
		int32_t eventsIndexed;

		int32_t textureCount;
		int32_t textureOffset;

		int32_t textureDirCount;
		int32_t textureDirOffset;

		int32_t skinRefCount;
		int32_t skinFamilyCount;
		int32_t skinRefOffset;

		int32_t bodypartCount;
		int32_t bodypartOffset;

		int32_t attachmentCount;
		int32_t attachmentOffset;

		int32_t localnodeCount;
		int32_t localnodeOffset;
		int32_t localnodeNameOffset;

		int32_t flexDescCount;
		int32_t flexDescOffset;

		int32_t flexControllerCount;
		int32_t flexControllerOffset;

		int32_t flexRulesCount;
		int32_t flexRulesOffset;

		int32_t ikChainCount;
		int32_t ikChainOffset;

		int32_t mouthsCount;
		int32_t mouthsOffset;

		int32_t localPoseParamCount;
		int32_t localPoseParamIndex;

		int32_t surfacePropIndex;

		int32_t keyvalueOffset;
		int32_t keyvalueCount;

		int32_t ikLockCount;
		int32_t ikLockIndex;

		float mass;
		int32_t contents; // Valve dev wiki doesn't know what this is but most likely contents flags https://wiki.facepunch.com/gmod/Enums/CONTENTS

		int32_t includeModelCount;
		int32_t includeModelOffset;

		int32_t virtualModel;

		int32_t animBlocksNameOffset;
		int32_t animBlocksCount;
		int32_t animBlocksOffset;

		int32_t animBlockModel;

		int32_t boneTableNameOffset;

		int32_t vertexBase;
		int32_t offsetBase;

		uint8_t directionalDotProduct;

		uint8_t rootLoD;

		uint8_t numAllowedRootLoDs;

		uint8_t unused0;
		int32_t unused1;

		int32_t flexControllerUICount;
		int32_t flexControllerUIOffset;

		float vertAnimFixedPointScale;
		int32_t unused2;

		int32_t header2Offset;
		int32_t unused3;
	};

	struct Header2
	{
		int32_t srcBoneTransformCount;
		int32_t srcBoneTransformOffset;

		int32_t illumPositionAttachmentOffset;

		float maxEyeDeflection;

		int32_t linearBoneOffset;

		int32_t szNameIndex;

		int32_t boneFlexDriverCount;
		int32_t boneFlexDriverOffset;

		int32_t reserved[56];
	};

	struct Bone
	{
		int32_t szNameIndex; // ??
		int32_t parent;
		int32_t boneControllers[6];

		Vector pos;
		Quaternion quat;
		RadianEuler rot;

		Vector posScale;
		Vector rotScale;

		Matrix3x4 poseToBone;
		Quaternion qAlignment;

		int32_t flags;

		int32_t procType;
		int32_t procIndex;

		int32_t physicsBoneIndex;
		int32_t surfacePropIndex;

		int32_t contents;

		int32_t unused[8];
	};
}

namespace VVDStructs
{
	using MDLStructs::ByteVector;

	using MDLStructs::Vector2D;
	using MDLStructs::Vector;
	using MDLStructs::Vector4D;

	struct Header
	{
		static const int32_t SUPPORTED_VERSION = 4;

		int32_t id;
		int32_t version;
		int32_t checksum;

		int32_t numLoDs;
		int32_t numLoDVertices[MAX_NUM_LODS];

		int32_t numFixups;
		int32_t fixupTableOffset;

		int32_t vertexDataOffset;
		int32_t tangentDataOffset;
	};

	struct Fixup
	{
		int32_t lod;
		int32_t sourceVertexId;
		int32_t numVertices;
	};

	struct BoneWeight
	{
		float weight[MAX_NUM_BONES_PER_VERT];
		int8_t bone[MAX_NUM_BONES_PER_VERT];
		uint8_t numBones;
	};

	struct Vertex
	{
		BoneWeight boneWeights;
		Vector pos;
		Vector normal;
		Vector2D texCoord;
	};
}

namespace VTXStructs
{
	struct Header
	{
		static const int32_t SUPPORTED_VERSION = 7;

		int32_t version;

		int32_t vertCacheSize;
		uint16_t maxBonesPerStrip;
		uint16_t maxBonesPerTri;
		int32_t maxBonesPerVert;

		int32_t checksum;

		int32_t numLoDs;

		int32_t materialReplacementListOffset;

		int32_t numBodyParts;
		int32_t bodyPartOffset;
	};

	struct BodyPartHeader
	{
		int32_t numModels;
		int32_t modelOffset;
	};

	struct ModelHeader
	{
		int32_t numLoDs;
		int32_t lodOffset;
	};

	struct ModelLoDHeader
	{
		int32_t numMeshes;
		int32_t meshOffset;

		float switchPoint;
	};

	struct MeshHeader
	{
		int32_t numStripGroups;
		int32_t stripGroupHeaderOffset;

		uint8_t flags;
	};

	struct StripGroupHeader
	{
		int32_t numVerts;
		int32_t vertOffset;

		int32_t numIndices;
		int32_t indexOffset;

		int32_t numStrips;
		int32_t stripOffset;

		uint8_t flags;
		
		// this struct has 2 more ints if the MDL version is >= 49, however sdk 2013 uses version 48 (does gmod backport 49?)
	};

	struct StripHeader
	{
		int32_t numIndices;
		int32_t indexOffset;

		int32_t numVerts;
		int32_t vertOffset;

		int16_t numBones;

		uint8_t flags;

		// this struct has 2 more ints if the MDL version is >= 49, however sdk 2013 uses version 48 (does gmod backport 49?)
	};

	// would otherwise be padded to 10 bytes
	struct __attribute__((packed)) Vertex
	{
		uint8_t boneWeightIndex[3];
		uint8_t numBones;

		uint16_t origMeshVertId;

		int8_t boneId[3];
	};
}
