#include "input.h"

#include <iostream>
#include <string>
#include <vector>

namespace Input 
{
	GLFWwindow* window;
	bool* heldKeys = new bool[GLFW_KEY_LAST - GLFW_KEY_SPACE];

	void ProcessInput(GLFWwindow* window, int key, int action)
	{
		int index = key - GLFW_KEY_SPACE;
		if (action == GLFW_PRESS)
			heldKeys[index] = true;
		else if (action == GLFW_RELEASE)
			heldKeys[index] = false;
	}

	bool IsKeyDown(int keycode)
	{
		int index = keycode - GLFW_KEY_SPACE;
		return (heldKeys[index] == true);
	}

	bool IsKeyPressed(int keycode) 
	{
		return glfwGetKey(window, keycode) == GLFW_PRESS;
	}

	bool IsKeyReleased(int keycode)
	{
		return glfwGetKey(window, keycode) == GLFW_RELEASE;
	}
}