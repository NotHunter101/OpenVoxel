#include "camera.h"

namespace Rendering
{
	void Camera::UpdateProjectionMatrix(float fov, float near, float far, float windowWidth, float windowHeight) 
	{
		this->fov = fov;
		this->near = near;
		this->far = far;

		float aspect = windowWidth / windowHeight;
		this->projectionMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
	}

	void Camera::UpdateViewMatrix() 
	{
		this->viewMatrix = glm::translate(glm::mat4(1.0f), -this->openObject->transform->position);
		this->viewMatrix = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		this->viewMatrix = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		this->viewMatrix = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void Camera::Update(float delta)
	{
		UpdateViewMatrix();
	}

	float Camera::GetFov() 
	{
		return this->fov;
	}

	float Camera::GetNear()
	{
		return this->near;
	}

	float Camera::GetFar()
	{
		return this->far;
	}
}