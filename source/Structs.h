#pragma once

#include "Enums.h"
#include "Limits.h"

#include <cstdint>

#define STRUCT_GETTER(classname, offset)                                                            \
inline const classname* Get##classname(const int i) const                                           \
{                                                                                                   \
	return reinterpret_cast<const classname*>(reinterpret_cast<const uint8_t*>(this) + offset) + i; \
}

// Common structs
namespace MDLStructs
{
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
}

namespace VVDStructs
{
	using MDLStructs::MAX_NUM_LODS;
	using MDLStructs::MAX_NUM_BONES_PER_VERT;

	using MDLStructs::Vector2D;
	using MDLStructs::Vector;
	using MDLStructs::Vector4D;

	struct __attribute__((packed)) Header
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

	struct __attribute__((packed)) Fixup
	{
		int32_t lod;
		int32_t sourceVertexId;
		int32_t numVertices;
	};

	struct __attribute__((packed)) BoneWeight
	{
		float weight[MAX_NUM_BONES_PER_VERT];
		int8_t bone[MAX_NUM_BONES_PER_VERT];
		uint8_t numBones;
	};

	struct __attribute__((packed)) Vertex
	{
		BoneWeight boneWeights;
		Vector pos;
		Vector normal;
		Vector2D texCoord;
	};
}

namespace VTXStructs
{
	struct __attribute__((packed)) Vertex
	{
		uint8_t boneWeightIndex[3];
		uint8_t numBones;

		uint16_t origMeshVertId;

		int8_t boneId[3];
	};

	struct __attribute__((packed)) Strip
	{
		int32_t numIndices;
		int32_t indexOffset;

		int32_t numVerts;
		int32_t vertOffset;

		int16_t numBones;

		VTXEnums::StripFlags flags;

		int32_t numBoneStateChanges;
		int32_t boneStateChangeOffset;

		// this struct has 2 more ints if the MDL version is >= 49, however sdk 2013 uses version 48 (does gmod backport 49?)
	};

	struct __attribute__((packed)) StripGroup
	{
		int32_t numVerts;
		int32_t vertOffset;
		STRUCT_GETTER(Vertex, vertOffset)

		int32_t numIndices;
		int32_t indexOffset;
		inline const uint16_t* GetIndex(const int i) const
		{
			return reinterpret_cast<const uint16_t*>(reinterpret_cast<const uint8_t*>(this) + indexOffset) + i;
		}

		int32_t numStrips;
		int32_t stripOffset;
		STRUCT_GETTER(Strip, stripOffset)

		VTXEnums::StripGroupFlags flags;

		// this struct has 2 more ints if the MDL version is >= 49, however sdk 2013 uses version 48 (does gmod backport 49?)
	};

	struct __attribute__((packed)) Mesh
	{
		int32_t numStripGroups;
		int32_t stripGroupHeaderOffset;
		STRUCT_GETTER(StripGroup, stripGroupHeaderOffset)

		VTXEnums::MeshFlags flags;
	};

	struct __attribute__((packed)) ModelLoD
	{
		int32_t numMeshes;
		int32_t meshOffset;
		STRUCT_GETTER(Mesh, meshOffset)

		float switchPoint;
	};

	struct __attribute__((packed)) Model
	{
		int32_t numLoDs;
		int32_t lodOffset;
		STRUCT_GETTER(ModelLoD, lodOffset)
	};

	struct __attribute__((packed)) BodyPart
	{
		int32_t numModels;
		int32_t modelOffset;
		STRUCT_GETTER(Model, modelOffset)
	};

	struct __attribute__((packed)) MaterialReplacement
	{
		int16_t materialId;
		int32_t replacementMaterialNameOffset;
		inline const char* GetMaterialReplacementName() const
		{
			return reinterpret_cast<const char*>(this) + replacementMaterialNameOffset;
		}
	};

	struct __attribute__((packed)) MaterialReplacementList
	{
		int32_t replacementCount;
		int32_t replacementOffset;
		STRUCT_GETTER(MaterialReplacement, replacementOffset)
	};

	struct __attribute__((packed)) Header
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
		STRUCT_GETTER(MaterialReplacementList, materialReplacementListOffset)

		int32_t numBodyParts;
		int32_t bodyPartOffset;
		STRUCT_GETTER(BodyPart, bodyPartOffset)
	};
}

namespace MDLStructs
{
	// These are not used and are only here for alignment
	// Pointers have been replaced with int32 to mimic a 32 bit system
	struct __attribute__((packed)) mstudio_modelvertexdata_t
	{
		int32_t pVertexData;
		int32_t pTangentData;
	};

	struct __attribute__((packed)) mstudio_meshvertexdata_t
	{
		int32_t modelvertexdata;
		int32_t numLODVertexes[MAX_NUM_LODS];
	};

	struct __attribute__((packed)) Bone
	{
		int32_t szNameIndex;
		inline const char* GetName()
		{
			return reinterpret_cast<const char*>(this) + szNameIndex;
		}

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

	struct __attribute__((packed)) Texture
	{
		int32_t szNameIndex;
		inline const char* GetName() const
		{
			return reinterpret_cast<const char*>(this) + szNameIndex;
		}

		int32_t flags;

		int32_t used; // ?????
		int32_t unused1;

		// Two pointers that are unused even in engine
		int32_t material;
		int32_t clientmaterial;

		int32_t unused[10];
	};

	struct __attribute__((packed)) Flex
	{
		int32_t flexDesc;

		float target0;
		float target1;
		float target2;
		float target3;

		int32_t vertsCount;
		int32_t vertsOffset;

		//STRUCT_GETTER(VertAnim, vertsOffset)
		//STRUCT_GETTER(VertAnimWrinkle, vertsOffset)

		int32_t flexPair;
		MDLEnums::VertAnimType vertAnimType;

		uint8_t unused0[3];
		int32_t unused1[6];
	};

	struct Model;
	struct __attribute__((packed)) Mesh
	{
		int32_t material;

		int32_t modelIndex;
		inline const Model* GetModel() const
		{
			return reinterpret_cast<const Model*>(reinterpret_cast<const uint8_t*>(this) + modelIndex);
		}

		int32_t vertsCount;
		int32_t vertsOffset;

		int32_t GetVertexIndex(const VTXStructs::Vertex* pVertex) const;
		int32_t GetTangentIndex(const VTXStructs::Vertex* pVertex) const;

		int32_t flexesCount;
		int32_t flexesOffset;
		STRUCT_GETTER(Flex, flexesOffset)

		int32_t materialType;
		int32_t materialParam;

		int32_t meshId;

		Vector centre;

		// Unused, only for alignment
		mstudio_meshvertexdata_t vertexdata;

		int unused[8];
	};

	struct __attribute__((packed)) Model
	{
		char name[64];

		int32_t type;

		float boundingRadius;

		int32_t meshesCount;
		int32_t meshesOffset;
		STRUCT_GETTER(Mesh, meshesOffset)

		int32_t vertsCount;
		int32_t vertsOffset;
		int32_t tangentsOffset;

		int32_t attachmentsCount;
		int32_t attachmentsOffset;

		int32_t eyeballsCount;
		int32_t eyeballsOffset;
		//STRUCT_GETTER(Eyeball, eyeballsOffset)

		// Unused, only for alignment
		mstudio_modelvertexdata_t vertexdata;

		int32_t unused[8];
	};

	struct __attribute__((packed)) BodyPart
	{
		int32_t szNameIndex;
		inline const char* GetName() const
		{
			return reinterpret_cast<const char*>(this) + szNameIndex;
		}

		int32_t modelsCount;
		int32_t base;
		int32_t modelsOffset;

		STRUCT_GETTER(Model, modelsOffset)
	};

	struct __attribute__((packed)) Header
	{
		static const int32_t SUPPORTED_VERSION = 48;

		int32_t id; // Model format ID (IDST)
		int32_t version;
		int32_t checksum;
		char name[64];
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
		STRUCT_GETTER(Bone, boneOffset)

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
		STRUCT_GETTER(Texture, textureOffset)

		int32_t textureDirCount;
		int32_t textureDirOffset;
		inline const char* GetTextureDirectory(const int i) const
		{
			return reinterpret_cast<const char*>(this) + *(reinterpret_cast<const int32_t*>(
				(reinterpret_cast<const uint8_t*>(this) + textureDirOffset)
			) + i);
		}

		int32_t skinRefCount;
		int32_t skinFamilyCount;
		int32_t skinRefOffset;
		inline int16_t GetSkin(const int family, const int material) const
		{
			const int16_t* skinTbl = reinterpret_cast<const int16_t*>(reinterpret_cast<const uint8_t*>(this) + skinRefOffset);
			return skinTbl[family * skinRefCount + material];
		}

		int32_t bodypartCount;
		int32_t bodypartOffset;
		STRUCT_GETTER(BodyPart, bodypartOffset)

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

	struct __attribute__((packed)) Header2
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

	inline int32_t Mesh::GetVertexIndex(const VTXStructs::Vertex* pVertex) const
	{
		return static_cast<int32_t>(pVertex->origMeshVertId) + vertsOffset + GetModel()->vertsOffset / sizeof(VVDStructs::Vertex);
	}
	inline int32_t Mesh::GetTangentIndex(const VTXStructs::Vertex* pVertex) const
	{
		return static_cast<int32_t>(pVertex->origMeshVertId) + vertsOffset + GetModel()->tangentsOffset / sizeof(Vector4D);
	}
}

#undef STRUCT_GETTER
