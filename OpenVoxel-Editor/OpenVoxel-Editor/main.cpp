#include "main.h"
#include "shader.h"
#include "render.h"
#include "voxel.h"
#include "engine.h"
#include "editor.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

namespace Game 
{
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    GLFWwindow* window;
    Editor::EditorApplication* application;

    Engine::SharedPointer<Voxel::VoxelWorld>* voxelWorld;

    void InitializeGameAndWorld()
    {
        float width = 1200.0f;
        float height = 720.0f;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        glViewport(0, 0, width, height);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CW);
        glEnable(GL_CULL_FACE);

        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

        application = new Editor::EditorApplication();
        Editor::ObjectView* objectView = Editor::CreateWindow<Editor::ObjectView>();
        Editor::ObjectInspector* objectInspector = Editor::CreateWindow<Editor::ObjectInspector>();

        std::cout << "Initalizing Voxel Memory\n";

        Engine::SetActiveScene(new Engine::OpenScene());

        Engine::SharedPointer<Engine::OpenObject>* worldObject = Engine::SceneInstance->AddObject("VoxelWorld");
        voxelWorld = Engine::CreateComponent<Voxel::VoxelWorld>(worldObject);

        glm::vec3 defaultOffset = glm::vec3(0.0f, 0.0f, -250.0f);

        Engine::SharedPointer<Engine::OpenObject>* meshObject = Engine::SceneInstance->AddObject("VoxelMesh");
        Engine::OpenObject::SetParent(meshObject, worldObject);

        meshObject->Pointer()->transform()->position = defaultOffset;
        meshObject->Pointer()->transform()->scale = glm::vec3(1.0f);

        Engine::SharedPointer<Voxel::VoxelMesh>* voxelMesh = Engine::CreateComponent<Voxel::VoxelMesh>(meshObject);
        voxelMesh->Pointer()->InitalizeBuffer(glm::uvec3(150, 150, 150));
        voxelWorld->Pointer()->AddMesh(voxelMesh);

        for (int x2 = 0; x2 < 150; x2++) {
            for (int y2 = 0; y2 < 150; y2++) {
                for (int z2 = 0; z2 < 150; z2++) {
                    voxelMesh->Pointer()->voxels[voxelMesh->Pointer()->PositionToPackedIndex(x2, y2, z2)] = 1;
                }
            }
        }

        std::cout << "Finished Initializing Voxel Memory\n";

        // TODO: Fix the hardcoding on this by having a build action which copies the shaders into the build folder
        if (Rendering::InitRenderer(voxelWorld, width, height, "C:\\Users\\teamt\\Documents\\GitHub\\OpenVoxel\\OpenVoxel-Sharp\\Shaders\\") == -1) {
            std::cout << "Rendering Init Failed!\n";
            return;
        }
    }

    void StartGameLoop() 
    {
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
            ProcessInput(window);

            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();

            ImGui::ShowDemoWindow(&showingDemoWindow);
            application->RenderWindows();

            float time = TimeSinceStart();
            deltaWriteTimer += delta;
            deltaWriteCounter++;

            voxelWorld->Pointer()->openObject()->GetChild(0)
                ->Pointer()->transform()->eulerRotation = glm::vec3(45.0f, sinf(time) * 180.0f, 0.0f);

            if (deltaWriteTimer >= 1.0f)
            {
                float avgDelta = deltaWriteTimer / deltaWriteCounter;
                deltaWriteTimer = 0.0f;
                deltaWriteCounter = 0;
                std::cout << "FPS: " + std::to_string(1.0f / avgDelta) + "\n";
            }

            Engine::SceneInstance->Update(delta);

            ImGui::Render();
            Rendering::RenderFrame();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            delta = TimeSinceStart() - time;
        }

        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();

        application->Destroy();
        Engine::SceneInstance->Destroy();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    float TimeSinceStart()
    {
        std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
        std::chrono::milliseconds msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - start);
        return msSinceStart.count() / 1000.0f;
    }

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);

        if (width != 0 && height != 0)
            Rendering::ChangeWindowSize(width, height);
    }

    void ProcessInput(GLFWwindow* window)
    {
        //if (io.WantCaptureKeyboard)
        //    return;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
}