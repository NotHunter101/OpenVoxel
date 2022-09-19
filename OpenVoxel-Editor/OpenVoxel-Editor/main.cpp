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

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    std::cout << "Initalizing Voxel Memory\n";

    Engine::OpenWorld* world = new Engine::OpenWorld();
    Engine::OpenObject* worldObject = new Engine::OpenObject("VoxelWorld");
    Voxel::VoxelWorld* voxelWorld = Engine::CreateComponent<Voxel::VoxelWorld>(worldObject);

    glm::vec3 defaultOffset = glm::vec3(0.0f, 0.0f, -400.0f);

    Engine::OpenObject* meshObject = new Engine::OpenObject("VoxelMesh");
    meshObject->transform->position = defaultOffset;
    meshObject->transform->scale = glm::vec3(1.0f);

    Voxel::VoxelMesh* voxelMesh = Engine::CreateComponent<Voxel::VoxelMesh>(meshObject);
    voxelMesh->InitalizeBuffer(glm::uvec3(200, 200, 200));
    voxelWorld->AddMesh(voxelMesh);

    for (int x2 = 0; x2 < 200; x2++) {
        for (int y2 = 0; y2 < 200; y2++) {
            for (int z2 = 0; z2 < 200; z2++) {
                voxelMesh->voxels[voxelMesh->PositionToPackedIndex(x2, y2, z2)] = 1;
            }
        }
    }

    std::cout << "Finished Initializing Voxel Memory\n";

    if (Rendering::InitRenderer(voxelWorld, width, height, "./Shaders/") == -1) {
        std::cout << "Rendering Init Failed!\n";
        return -1;
    }

    std::cout << "About to start rendering...\n";

    float delta = 0.0f;
    float deltaWriteTimer = 0.0f;
    int deltaWriteCounter = 0.0f;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable some options

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(NULL);

    bool showingDemoWindow = true;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window, io);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow(&showingDemoWindow);

        float time = timeSinceStart();
        deltaWriteTimer += delta;
        deltaWriteCounter++;

        meshObject->transform->eulerRotation = glm::vec3(45.0f, sin(time) * 180.0f, 0.0f);
        meshObject->transform->scale = glm::vec3(sin(time) * 0.5f + 0.5f, sin(time) * 0.5f + 0.5f, sin(time) * 0.5f + 0.5f);

        if (deltaWriteTimer >= 1.0f)
        {
            float avgDelta = deltaWriteTimer / deltaWriteCounter;
            deltaWriteTimer = 0.0f;
            deltaWriteCounter = 0;
            std::cout << "FPS: " + std::to_string(1.0f / avgDelta) + "\n";
        }

        world->Update(delta);

        ImGui::Render();
        Rendering::RenderFrame();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        delta = timeSinceStart() - time;
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    world->Destroy();

    glfwDestroyWindow(window);
    glfwTerminate();

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

void processInput(GLFWwindow* window, ImGuiIO io)
{
    if (io.WantCaptureKeyboard)
        return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}