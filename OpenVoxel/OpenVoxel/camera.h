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
		using Engine::Component::Component;

		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		void UpdateProjectionMatrix(float fov, float near, float far, float windowWidth, float windowHeight);
		void UpdateViewMatrix();
		void Update(float delta) override;

		float GetFov();
		float GetNear();
		float GetFar();
	};
}