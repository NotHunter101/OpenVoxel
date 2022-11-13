#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace Game
{
	void InitializeGameAndWorld();
	void StartGameLoop();
	float TimeSinceStart();
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	void ProcessInput(GLFWwindow* window, int key, int scancode, int action, int mods);
}