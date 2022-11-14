#include "camera.h"
#include "input.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

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
		this->translateMatrix = glm::translate(glm::mat4(1.0f), -this->openObject()->transform()->position);
		this->rotateMatrix = glm::rotate(glm::mat4(1.0f), -this->openObject()->transform()->eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		this->rotateMatrix = glm::rotate(this->rotateMatrix, -this->openObject()->transform()->eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		this->rotateMatrix = glm::rotate(this->rotateMatrix, -this->openObject()->transform()->eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	
		this->viewMatrix = this->rotateMatrix * this->translateMatrix;
	}

	void Camera::Update(float delta)
	{
		UpdateViewMatrix();
	}

	void PlayerController::Awake()
	{
		this->childCamera = openObject()->GetChild(0)->Ptr()->GetComponent<Camera>();
	}

	void PlayerController::Update(float delta)
	{
		glm::mat4 rotationMatrix = this->childCamera->Ptr()->rotateMatrix;
		glm::vec3 cameraForward = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f) * rotationMatrix;
		glm::vec3 cameraRight = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) * rotationMatrix;

		float moveSpeed = 20.0f;

		if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
			moveSpeed = 200.0f;
		}

		float deltaMoveSpeed = moveSpeed * delta;

		float rotateSpeed = 1.0f;
		float deltarotateSpeed = rotateSpeed * delta;

		if (Input::IsKeyDown(GLFW_KEY_W)) {
			this->childCamera->Ptr()->transform()->position += 
				cameraForward * deltaMoveSpeed;
		}
		if (Input::IsKeyDown(GLFW_KEY_S)) {
			this->childCamera->Ptr()->transform()->position -= 
				cameraForward * deltaMoveSpeed;
		}
		if (Input::IsKeyDown(GLFW_KEY_D)) {
			this->childCamera->Ptr()->transform()->position += 
				cameraRight * deltaMoveSpeed;
		}
		if (Input::IsKeyDown(GLFW_KEY_A)) {
			this->childCamera->Ptr()->transform()->position -= 
				cameraRight * deltaMoveSpeed;
		}

		if (Input::IsKeyDown(GLFW_KEY_UP)) {
			this->childCamera->Ptr()->transform()->eulerRotation += 
				glm::vec3(deltarotateSpeed, 0.0f, 0.0f);
		}
		if (Input::IsKeyDown(GLFW_KEY_DOWN)) {
			this->childCamera->Ptr()->transform()->eulerRotation -=
				glm::vec3(deltarotateSpeed, 0.0f, 0.0f);
		}
		if (Input::IsKeyDown(GLFW_KEY_LEFT)) {
			this->childCamera->Ptr()->transform()->eulerRotation +=
				glm::vec3(0.0f, deltarotateSpeed, 0.0f);
		}
		if (Input::IsKeyDown(GLFW_KEY_RIGHT)) {
			this->childCamera->Ptr()->transform()->eulerRotation -=
				glm::vec3(0.0f, deltarotateSpeed, 0.0f);
		}

		//if ()

		/*f = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		this->viewMatrix = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		this->viewMatrix = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 cameraForward = this->childCamera->transform->eulerRotation;*/
	}
}