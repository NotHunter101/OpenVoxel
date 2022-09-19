#pragma once

#include "voxel.h";
#include "shader.h"

namespace Rendering 
{
	int InitRenderer(Voxel::VoxelWorld* world, float width, float height, std::string shaderPath);
	void RenderFrame();
	void SetVoxels(int meshIndex);
	void SetVisibleVoxels(int meshIndex);
	void SetMVP(Voxel::VoxelMesh* mesh, ShaderProgram program);
	void SetFaceRotationMatrices(ShaderProgram program);
	void SetVoxelBufferDimensions(Voxel::VoxelMesh* mesh, ShaderProgram program);
}