#include "shader.h"
#include "render.h"
#include "camera.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <string>

namespace Rendering 
{
    Voxel::VoxelWorld* voxelWorld;
    ShaderProgram* shaderPrograms;

    unsigned int* VAOs;
    unsigned int* VBOs;
    int* visibleVoxelCount;

    Camera* targetCamera;

    int InitRenderer(Voxel::VoxelWorld* world, float width, float height)
    {
        voxelWorld = world;

        Engine::OpenObject* cameraObject = new Engine::OpenObject("Camera");
        targetCamera = Engine::CreateComponent<Camera>(cameraObject);
        targetCamera->openObject->transform->position = glm::vec3(0.0f, 0.0f, 4.0f);

        targetCamera->UpdateProjectionMatrix(60.0f, 0.01f, 100.0f, width, height);
        targetCamera->UpdateViewMatrix();

        Shader vertexShader = Shader("Vertex", GL_VERTEX_SHADER, "vertex.glsl");
        Shader fragmentShader = Shader("Fragment", GL_FRAGMENT_SHADER, "fragment.glsl");
        Shader geometryShader = Shader("Geometry", GL_GEOMETRY_SHADER, "geometry.glsl");

        if (!vertexShader.compiled || !fragmentShader.compiled || !geometryShader.compiled) {
            return -1;
        }

        shaderPrograms = new ShaderProgram[world->meshCount];

        for (int i = 0; i < world->meshCount; i++) {
            ShaderProgram shaderProgram = ShaderProgram(&vertexShader, &fragmentShader, &geometryShader);

            if (!shaderProgram.linked) {
                return -1;
            }

            shaderPrograms[i] = shaderProgram;
        }

        std::cout << "Line 50...\n";

        VAOs = new unsigned int[voxelWorld->meshCount];
        VBOs = new unsigned int[voxelWorld->meshCount];
        visibleVoxelCount = new int[voxelWorld->meshCount];

        for (int i = 0; i < voxelWorld->meshCount; i++) 
        {
            glGenBuffers(1, &VBOs[i]);
            glGenVertexArrays(1, &VAOs[i]);
            glBindVertexArray(VAOs[i]);
            SetVisibleVoxels(i);

            glVertexAttribPointer(0, 1, GL_DOUBLE, GL_FALSE, sizeof(uint64_t), (void*)0);
            glEnableVertexAttribArray(0);
        }

        return 0;
    }

	void RenderFrame()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < voxelWorld->meshCount; i++) 
        {
            Voxel::VoxelMesh* mesh = voxelWorld->meshes[i];
            shaderPrograms[i].UseProgram();
            SetMVP(mesh, shaderPrograms[i]);

            glBindVertexArray(VAOs[i]);
            if (mesh->bufferDirty)
                SetVisibleVoxels(i);

            glDrawArrays(GL_POINTS, 0, visibleVoxelCount[i]);
            glBindVertexArray(0);
        }
	}

    void SetVisibleVoxels(int meshIndex) 
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[meshIndex]);
        uint64_t* visibleVoxels = voxelWorld->meshes[meshIndex]->GetVisibleVoxels(visibleVoxelCount[meshIndex]);
        
        uint64_t maxValue = uint64_t(pow(2, 8) - 1);
        uint8_t yPos = (visibleVoxels[0] & (maxValue << 8)) >> 8;
        std::cout << int(yPos);
        std::cout << std::endl;
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(visibleVoxels), visibleVoxels, GL_STATIC_DRAW);
        delete[] visibleVoxels;
    }

    void SetMVP(Voxel::VoxelMesh* mesh, ShaderProgram program) 
    {
        program.SetMat4x4("MVP_MATRIX", targetCamera->projectionMatrix * targetCamera->viewMatrix * mesh->GetModelMatrix());
    }
}