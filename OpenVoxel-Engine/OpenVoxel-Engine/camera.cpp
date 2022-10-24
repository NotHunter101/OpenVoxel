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
		this->translateMatrix = glm::translate(glm::mat4(1.0f), -this->openObject()->transform()->position);
		this->rotateMatrix = glm::rotate(glm::mat4(1.0f), -this->openObject()->transform()->eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		this->rotateMatrix = glm::rotate(this->rotateMatrix, -this->openObject()->transform()->eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		this->rotateMatrix = glm::rotate(this->rotateMatrix, -this->openObject()->transform()->eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	
		this->viewMatrix = this->translateMatrix * this->rotateMatrix;
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
		glm::vec3 cameraFoward = this->childCamera->Ptr()->rotateMatrix 
			* glm::vec4(this->childCamera->Ptr()->transform()->position, 1.0f);

		//if ()

		/*f = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		this->viewMatrix = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		this->viewMatrix = glm::rotate(this->viewMatrix, -this->openObject->transform->eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 cameraForward = this->childCamera->transform->eulerRotation;*/
	}
}