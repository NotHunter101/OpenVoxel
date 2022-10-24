#pragma once

#include "engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Rendering 
{
	class Camera : public Engine::Component
	{
	private:
		float fov;
		float near;
		float far;
	public:
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		glm::mat4 rotateMatrix;
		glm::mat4 translateMatrix;

		void UpdateProjectionMatrix(float fov, float near, float far, float windowWidth, float windowHeight);
		void UpdateViewMatrix();
		void Update(float delta) override;
	};

	class PlayerController : public Engine::Component
	{
	private:
		Engine::SharedPtr<Camera>* childCamera;
	public:
		void Awake() override;
		void Update(float delta) override;
	};
}