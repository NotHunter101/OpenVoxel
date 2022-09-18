#pragma once

#include "engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Voxel
{
	int PositionToPackedIndex(int x, int y, int z);
	int PackedIndexToPosition(int packedIndex);

	struct VoxelLightValue 
	{
		uint8_t rLight;
		uint8_t gLight;
		uint8_t bLight;
	};

	class VoxelLightmap : public Engine::Component
	{
	public:
		using Engine::Component::Component;

		VoxelLightValue* lightValues;

		void Awake() override;
	};

	class VoxelMesh : public Engine::Component
	{
	public:
		using Engine::Component::Component;

		int voxelArraySize;
		std::uint8_t* voxels;
		bool bufferDirty;

		void Awake() override;

		// Must be called every time there is a change in the "voxels" array
		uint64_t* GetVisibleVoxels(int& count);
		glm::mat4 GetModelMatrix();
		void Destroy() override;
	};

	class VoxelWorld : public Engine::Component
	{
	public:
		using Engine::Component::Component;
		
		VoxelLightmap lightmap;
		VoxelMesh** meshes;
		int meshCount;

		void Awake() override;
		void AddMesh(VoxelMesh* mesh);
		void Destroy() override;
	};
}