#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

float timeSinceStart();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, ImGuiIO IO);