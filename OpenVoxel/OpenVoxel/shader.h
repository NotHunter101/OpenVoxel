#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <string>

namespace Rendering 
{
	class Shader 
	{
	public:
		unsigned int ID;
		bool compiled;
		Shader(std::string shaderTypeName, GLenum shaderType, const char* sourcePath);

		void Delete();
	};

	class ShaderProgram 
	{
	public:
		unsigned int ID;
		bool linked;
		ShaderProgram(Shader* vertexShader, Shader* fragmentShader, Shader* geometryShader);
		ShaderProgram();
		
		void UseProgram();
		void SetMat4x4(const char* uniformName, glm::mat4);
	};
}