#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

float timeSinceStart();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);