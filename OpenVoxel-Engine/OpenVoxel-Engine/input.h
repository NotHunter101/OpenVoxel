#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace Input 
{
	extern GLFWwindow* window;

	void ProcessInput(GLFWwindow* window, int key, int action);
	bool IsKeyDown(int keycode);
	bool IsKeyPressed(int keycode);
	bool IsKeyReleased(int keycode);
}