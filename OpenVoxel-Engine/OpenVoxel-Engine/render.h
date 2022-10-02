#pragma once

#include "voxel.h";
#include "shader.h"

namespace Rendering 
{
	int InitRenderer(Engine::SharedPointer<Voxel::VoxelWorld>* world, int width, int height, std::string shaderPath);
	void RenderFrame();
	void ChangeWindowSize(int width, int height);
	void SetVoxels(int meshIndex);
	void SetVisibleVoxels(int meshIndex);
	void SetMVP(Voxel::VoxelMesh* mesh, ShaderProgram program);
	void SetFaceRotationMatrices(ShaderProgram program);
	void SetVoxelBufferDimensions(Voxel::VoxelMesh* mesh, ShaderProgram program);
}