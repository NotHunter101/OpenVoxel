#include "main.h"
#include "shader.h"
#include "render.h"
#include "voxel.h"
#include "engine.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>

std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

int main()
{
    float width = 1200.0f;
    float height = 720.0f;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, width, height);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    //glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    std::cout << "Initalizing Voxel Memory\n";

    Engine::OpenWorld* world = new Engine::OpenWorld();
    Engine::OpenObject* worldObject = new Engine::OpenObject("VoxelWorld");
    Voxel::VoxelWorld* voxelWorld = Engine::CreateComponent<Voxel::VoxelWorld>(worldObject);
    Engine::OpenObject* meshObject = new Engine::OpenObject("VoxelMesh");
    Voxel::VoxelMesh* voxelMesh = Engine::CreateComponent<Voxel::VoxelMesh>(meshObject);
    voxelWorld->AddMesh(voxelMesh);
    voxelMesh->voxels[Voxel::PositionToPackedIndex(128, 128, 128)] = 1;

    std::cout << "Finished Initializing Voxel Memory\n";

    if (Rendering::InitRenderer(voxelWorld, width, height) == -1) {
        std::cout << "Rendering Init Failed!\n";
        return -1;
    }

    std::cout << "About to start rendering...\n";

    float delta = 0.0f;
    float deltaWriteTimer = 0.0f;
    int deltaWriteCounter = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float time = timeSinceStart();
        deltaWriteTimer += delta;
        deltaWriteCounter++;

        meshObject->transform->position = glm::vec3(0.0f, 0.0f, sinf(time));
        meshObject->transform->eulerRotation = glm::vec3(45.0f + sinf(time) * 90.0f, sinf(time) * 180.0f, 0.0f);

        if (deltaWriteTimer >= 1.0f) 
        {
            float avgDelta = deltaWriteTimer / deltaWriteCounter;
            deltaWriteTimer = 0.0f;
            deltaWriteCounter = 0;
            std::cout << "FPS: " + std::to_string(1.0f / avgDelta) + "\n";
        }

        processInput(window);
        world->Update(delta);
        Rendering::RenderFrame();
        glfwPollEvents();
        glfwSwapBuffers(window);
        delta = timeSinceStart() - time;
    }

    world->Destroy();
    return 0;
}

float timeSinceStart() 
{
    std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
    std::chrono::milliseconds msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - start);
    return msSinceStart.count() / 1000.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}