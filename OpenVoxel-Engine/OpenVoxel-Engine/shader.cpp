#include "shader.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <fstream>
#include <streambuf>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rendering 
{
    Shader::Shader(std::string shaderTypeName, GLenum shaderType, std::string sourcePath)
    {
        std::ifstream shaderStream(sourcePath);
        std::string shaderSourceStr((std::istreambuf_iterator<char>(shaderStream)),
            std::istreambuf_iterator<char>());
        const char* shaderSource = shaderSourceStr.c_str();

        this->ID = glCreateShader(shaderType);
        glShaderSource(this->ID, 1, &shaderSource, NULL);
        glCompileShader(this->ID);

        int success;
        char infoLog[512];
        glGetShaderiv(this->ID, GL_COMPILE_STATUS, &success);
        this->compiled = success;

        if (!success)
        {
            glGetShaderInfoLog(this->ID, 512, NULL, infoLog);
            std::cout << "Shader Compilation Failed! (" + shaderTypeName + ")\n" << infoLog << std::endl;
            this->Delete();
        }
    }

    void Shader::Delete() 
    {
        glDeleteShader(this->ID);
    }

    ShaderProgram::ShaderProgram(Shader* vertexShader, Shader* fragmentShader, Shader* geometryShader) 
    {
        this->ID = glCreateProgram();

        glAttachShader(this->ID, vertexShader->ID);
        glAttachShader(this->ID, fragmentShader->ID);

        if (geometryShader != nullptr) {
            glAttachShader(this->ID, geometryShader->ID);
        }
        
        glLinkProgram(this->ID);

        vertexShader->Delete();
        fragmentShader->Delete();

        if (geometryShader != nullptr) {
            geometryShader->Delete();
        }

        int success;
        char infoLog[512];
        glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
        this->linked = success;

        if (!success)
        {
            glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
            std::cout << "Shader Linking Failed!\n" << infoLog << std::endl;
        }
    }

    ShaderProgram::ShaderProgram()
    {
        this->ID = 0;
        this->linked = false;
    }

    void ShaderProgram::UseProgram()
    {
        glUseProgram(this->ID);
    }

    void ShaderProgram::SetMat4x4(const char* uniformName, glm::mat4 value)
    {
        glUniformMatrix4fv(glGetUniformLocation(this->ID, uniformName), 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::SetUInt(const char* uniformName, unsigned int value)
    {
        glUniform1ui(glGetUniformLocation(this->ID, uniformName), value);
    }
}