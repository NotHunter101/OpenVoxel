#pragma once

#include "engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Voxel
{
	struct VoxelLightValue 
	{
		uint8_t rLight;
		uint8_t gLight;
		uint8_t bLight;
	};

	class VoxelLightmap : public Engine::Component
	{
	public:
		VoxelLightValue* lightValues;

		void Awake() override;
	};

	class VoxelMesh : public Engine::Component
	{
	private:
		glm::uvec3 voxelArrayDimensions;
		int voxelArraySize;
	public:
		std::uint8_t* voxels;
		bool bufferDirty;

		void Awake() override;
		void InitalizeBuffer(glm::uvec3 bufferDimensions);

		unsigned int PositionToPackedIndex(unsigned int x, unsigned int y, unsigned int z);
		bool PositionInBounds(unsigned int x, unsigned int y, unsigned int z);
		std::vector<unsigned int>* GetVisibleVoxels();
		glm::mat4 GetModelMatrix();
		glm::uvec3 GetVoxelArrayDimensions();
		void Destroy() override;
	};

	class VoxelWorld : public Engine::Component
	{
	public:
		VoxelLightmap lightmap;
		std::vector<VoxelMesh*> meshes;
		int meshCount;

		void Awake() override;
		void AddMesh(VoxelMesh* mesh);
		void Destroy() override;
	};
}