#pragma once

#include "voxel.h";
#include "shader.h"

namespace Rendering 
{
	int InitRenderer(Voxel::VoxelWorld* world, float width, float height);
	void RenderFrame();
	void SetVisibleVoxels(int meshIndex);
	void SetMVP(Voxel::VoxelMesh* mesh, ShaderProgram program);
}