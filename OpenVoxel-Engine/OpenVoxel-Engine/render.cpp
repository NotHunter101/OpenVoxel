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
    unsigned int* EBOs;
    int* visibleVoxelCount;

    Camera* targetCamera;

    int InitRenderer(Voxel::VoxelWorld* world, float width, float height, std::string shaderPath)
    {
        voxelWorld = world;

        Engine::OpenObject* cameraObject = new Engine::OpenObject("Camera");
        targetCamera = Engine::CreateComponent<Camera>(cameraObject);
        targetCamera->openObject->transform->position = glm::vec3(0.0f, 0.0f, 4.0f);

        targetCamera->UpdateProjectionMatrix(60.0f, 0.1f, 500.0f, width, height);
        targetCamera->UpdateViewMatrix();

        Shader vertexShader = Shader("Vertex", GL_VERTEX_SHADER, shaderPath + "vertex.glsl");
        Shader fragmentShader = Shader("Fragment", GL_FRAGMENT_SHADER, shaderPath + "fragment.glsl");
        Shader geometryShader = Shader("Geometry", GL_GEOMETRY_SHADER, shaderPath + "geometry.glsl");

        if (!vertexShader.compiled || !fragmentShader.compiled || !geometryShader.compiled) {
            return -1;
        }

        shaderPrograms = new ShaderProgram[voxelWorld->meshCount];
        visibleVoxelCount = new int[voxelWorld->meshCount];

        VAOs = new unsigned int[voxelWorld->meshCount];
        VBOs = new unsigned int[voxelWorld->meshCount];
        EBOs = new unsigned int[voxelWorld->meshCount];

        for (int i = 0; i < voxelWorld->meshCount; i++) 
        {
            ShaderProgram shaderProgram = ShaderProgram(&vertexShader, &fragmentShader, &geometryShader);
            if (!shaderProgram.linked) {
                return -1;
            }

            shaderProgram.UseProgram();
            SetFaceRotationMatrices(shaderProgram);
            SetVoxelBufferDimensions(voxelWorld->meshes[i], shaderProgram);
            glUseProgram(0);

            shaderPrograms[i] = shaderProgram;

            glGenBuffers(1, &VBOs[i]);
            glGenBuffers(1, &EBOs[i]);
            glGenVertexArrays(1, &VAOs[i]);

            glBindVertexArray(VAOs[i]);
            SetVoxels(i);
            SetVisibleVoxels(i);

            glVertexAttribPointer(0, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
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
            {
                SetVoxels(i);
                SetVisibleVoxels(i);
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
            glDrawElements(GL_POINTS, visibleVoxelCount[i], GL_UNSIGNED_INT, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
	}

    void SetVoxels(int meshIndex) 
    {
        uint8_t* voxels = voxelWorld->meshes[meshIndex]->voxels;

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[meshIndex]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(voxels), voxels, GL_DYNAMIC_DRAW);
    }

    void SetVisibleVoxels(int meshIndex) 
    {
        std::vector<unsigned int>* visibleVoxels = voxelWorld->meshes[meshIndex]->GetVisibleVoxels();
        visibleVoxelCount[meshIndex] = visibleVoxels->size();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[meshIndex]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, visibleVoxelCount[meshIndex] * sizeof(unsigned int), visibleVoxels->data(), GL_DYNAMIC_DRAW);

        delete visibleVoxels;
    }

    void SetMVP(Voxel::VoxelMesh* mesh, ShaderProgram program) 
    {
        program.SetMat4x4("MVP_MATRIX", targetCamera->projectionMatrix * targetCamera->viewMatrix * mesh->GetModelMatrix());
    }

    void SetFaceRotationMatrices(ShaderProgram program)
    {
        float pi = 3.14159265358979323846;
        glm::vec3 rotations[6] = {
            glm::vec3(0, 0, 0),
            glm::vec3(pi, 0, 0),
            glm::vec3(0, 0, pi / 2.0f),
            glm::vec3(0, 0, -(pi / 2.0f)),
            glm::vec3(-(pi / 2.0f), 0, 0),
            glm::vec3(pi / 2.0f, 0, 0)
        };

        for (int i = 0; i < 6; i++) 
        {
            glm::mat4 rotatedFaceMat = glm::mat4(1.0f);
            if (rotations[i].x != 0) {
                rotatedFaceMat = glm::rotate(rotatedFaceMat, rotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            if (rotations[i].z != 0) {
                rotatedFaceMat = glm::rotate(rotatedFaceMat, rotations[i].z, glm::vec3(0.0f, 0.0f, 1.0f));
            }

            std::string buf("FACE_ROT_MATRICES[");
            buf.append(std::to_string(i));
            buf.append("]");
            program.SetMat4x4(buf.c_str(), rotatedFaceMat);
        }
    }

    void SetVoxelBufferDimensions(Voxel::VoxelMesh* mesh, ShaderProgram program)
    {
        glm::uvec3 bufferDimensions = mesh->GetVoxelArrayDimensions();
        program.SetUInt("MAXSIZE_X", bufferDimensions.x);
        program.SetUInt("MAXSIZE_Y", bufferDimensions.y);
        program.SetUInt("MAXSIZE_Z", bufferDimensions.z);
    }
}