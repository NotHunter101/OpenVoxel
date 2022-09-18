#include "voxel.h"
#include "camera.h"

#include <iostream>
#include <string>

namespace Voxel
{
	const int maxMeshSize = 255;

	int PositionToPackedIndex(int x, int y, int z)
	{
		return x + (y * maxMeshSize) + (z * maxMeshSize * maxMeshSize);
	}

	void VoxelLightmap::Awake()
	{
		// Max size of world currently is 1000x1000x1000, otherwise lightmap stops affecting geometry
		this->lightValues = new VoxelLightValue[pow(1000, 3)];
	}

	void VoxelMesh::Awake()
	{
		this->voxelArraySize = (int)pow(maxMeshSize, 3);
		this->voxels = new uint8_t[voxelArraySize];
		this->bufferDirty = false;

		for (int i = 0; i < voxelArraySize; i++) 
			this->voxels[i] = (uint8_t)0;
	}

	uint64_t* VoxelMesh::GetVisibleVoxels(int& count)
	{
		bool* neighbors = new bool[6];

		// top, bottom, right, left, back, front
		glm::vec3 offsets[] = {
			glm::vec3(0, 1, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, 1)
		};
		
		// at most, only half the voxels can be visible at any time
		int maxVisible = (int)truncf(powf(maxMeshSize, 3) * 0.5f);
		uint64_t* visible = new uint64_t[maxVisible];
		count = 0;

		for (int x = 0; x < maxMeshSize; x++) 
		{
			for (int y = 0; y < maxMeshSize; y++) 
			{
				for (int z = 0; z < maxMeshSize; z++) 
				{
					int myIndex = PositionToPackedIndex(x, y, z);
					uint8_t material = this->voxels[myIndex];

					if (material == 0)
						continue;

					bool anyFaceVisible = false;
					for (int i = 0; i < 6; i++)
					{
						glm::vec3 offset = offsets[i];
						int neighborIndex = PositionToPackedIndex(x + offset.x, y + offset.y, z + offset.z);
						bool faceVisible = false;
						
						if (neighborIndex >= 0 && neighborIndex < this->voxelArraySize)
							faceVisible = this->voxels[neighborIndex] == 0;
						
						neighbors[i] = faceVisible; // 0 = AIR
						if (faceVisible)
							anyFaceVisible = true;
					}

					if (!anyFaceVisible)
						continue;

					visible[count] = ((uint64_t)material << 54) | ((uint64_t)0 << 46) | ((uint64_t)0 << 38) | ((uint64_t)0 << 30) |
						((uint64_t)neighbors[5] << 29) | ((uint64_t)neighbors[4] << 28) | ((uint64_t)neighbors[3] << 27) |
						((uint64_t)neighbors[2] << 26) | ((uint64_t)neighbors[1] << 25) | ((uint64_t)neighbors[0] << 24) |
						((uint64_t)z << 16) | ((uint64_t)y << 8) | ((uint64_t)x);
					count++;
				}
			}
		}

		delete[] neighbors;
		return visible;
	}

	glm::mat4 VoxelMesh::GetModelMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::scale(model, this->openObject->transform->scale);
		model = glm::translate(model, this->openObject->transform->position);
		model = glm::rotate(model, glm::radians(this->openObject->transform->eulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(this->openObject->transform->eulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(this->openObject->transform->eulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		
		return model;
	}

	void VoxelMesh::Destroy()
	{
		delete[] this->voxels;
	}

	void VoxelWorld::Awake()
	{
		this->meshes = new VoxelMesh*[0];
		this->meshCount = 0;
	}

	void VoxelWorld::AddMesh(VoxelMesh* mesh)
	{
		VoxelMesh** oldMeshes = new VoxelMesh*[this->meshCount];
		int oldMeshCount = this->meshCount;

		for (int i = 0; i < this->meshCount; i++) {
			oldMeshes[i] = this->meshes[i];
		}

		this->meshCount++;
		this->meshes = new VoxelMesh*[this->meshCount];

		for (int i = 0; i < oldMeshCount; i++) {
			this->meshes[i] = oldMeshes[i];
		}

		this->meshes[this->meshCount - 1] = mesh;
	}

	void VoxelWorld::Destroy()
	{
		delete[] this->lightmap.lightValues;
		delete[] this->meshes;
	}
}