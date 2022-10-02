#include "voxel.h"
#include "camera.h"

#include <iostream>
#include <string>

namespace Voxel
{
	void VoxelLightmap::Awake()
	{
		// Max size of world currently is 1000x1000x1000, otherwise lightmap stops affecting geometry
		this->lightValues = new VoxelLightValue[pow(1000, 3)];
	}

	void VoxelLightmap::Destroy()
	{
		delete[] this->lightValues;
	}

	void VoxelMesh::Awake()
	{
		this->voxelArrayDimensions = glm::uvec3(0);
		this->voxelArraySize = 0;
		this->voxels = new uint8_t[0];
		this->bufferDirty = false;
	}

	void VoxelMesh::InitalizeBuffer(glm::uvec3 bufferDimensions) 
	{
		this->voxelArrayDimensions = bufferDimensions;
		this->voxelArraySize = (int)(bufferDimensions.x * bufferDimensions.y * bufferDimensions.z);
		this->voxels = new uint8_t[this->voxelArraySize];

		for (int i = 0; i < voxelArraySize; i++)
			this->voxels[i] = (uint8_t)0;
	}

	unsigned int VoxelMesh::PositionToPackedIndex(unsigned int x, unsigned int y, unsigned int z)
	{
		return x + (y * this->voxelArrayDimensions.x) + (z * this->voxelArrayDimensions.x * this->voxelArrayDimensions.y);
	}

	bool VoxelMesh::PositionInBounds(unsigned int x, unsigned int y, unsigned int z)
	{
		if (x < 0 || x >= this->voxelArrayDimensions.x)
			return false;
		if (y < 0 || y >= this->voxelArrayDimensions.y)
			return false;
		if (z < 0 || z >= this->voxelArrayDimensions.z)
			return false;

		return true;
	}

	std::vector<unsigned int>* VoxelMesh::GetVisibleVoxels()
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
		std::vector<unsigned int>* visible = new std::vector<unsigned int>();

		for (int x = 0; x < this->voxelArrayDimensions.x; x++) 
		{
			for (int y = 0; y < this->voxelArrayDimensions.y; y++)
			{
				for (int z = 0; z < this->voxelArrayDimensions.z; z++)
				{
					unsigned int myIndex = PositionToPackedIndex(x, y, z);
					uint8_t material = this->voxels[myIndex];

					if (material == 0)
						continue;

					for (int i = 0; i < 6; i++)
					{
						glm::vec3 offset = offsets[i];
						int neighborIndex = this->PositionToPackedIndex(x + offset.x, y + offset.y, z + offset.z);
						bool faceVisible = !this->PositionInBounds(x + offset.x, y + offset.y, z + offset.z) || this->voxels[neighborIndex] == 0;
						
						if (faceVisible)
						{
							visible->push_back(myIndex);
							break;
						}
					}
				}
			}
		}

		delete[] neighbors;
		return visible;
	}

	glm::mat4 VoxelMesh::GetModelMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::scale(model, this->openObject()->transform()->scale);
		model = glm::translate(model, this->openObject()->transform()->position);
		model = glm::rotate(model, glm::radians(this->openObject()->transform()->eulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(this->openObject()->transform()->eulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(this->openObject()->transform()->eulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		
		return model;
	}

	void VoxelMesh::Destroy()
	{
		delete[] this->voxels;
	}

	void VoxelWorld::Awake()
	{
		this->lightmap = new VoxelLightmap();
		this->meshes = std::vector<Engine::SharedPointer<VoxelMesh>*>();
		this->meshCount = 0;
	}

	void VoxelWorld::AddMesh(Engine::SharedPointer<VoxelMesh>* mesh)
	{
		this->meshes.push_back(mesh);
		this->meshCount++;
	}

	glm::uvec3 VoxelMesh::GetVoxelArrayDimensions()
	{
		return this->voxelArrayDimensions;
	}
}