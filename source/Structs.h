#pragma once

#include "Enums.h"

#include <cstdint>

namespace MDLStructs
{
	struct Vector
	{
		float x, y, z;
	};

	struct Quaternion
	{
		float x, y, z, w;
	};

	typedef Vector RadianEuler;

	struct Matrix3x4
	{
		float m[3][4];

		float* operator[](int i) { return m[i]; }
		const float* operator[](int i) const { return m[i]; }
	};

	struct Header
	{
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
