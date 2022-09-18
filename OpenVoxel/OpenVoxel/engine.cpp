#include "engine.h"

#include <sstream>
#include <iostream>
#include <string>

namespace Engine 
{
	OpenWorld* worldInstance;

	void OpenWorld::AddObject(OpenObject* component)
	{
		this->objects.push_back(component);
	}

	void OpenWorld::DeleteObject(OpenObject* component)
	{
		int index = 0;
		for (index = 0; index < this->objects.size(); index++) {
			if (component == this->objects[index]) {
				break;
			}
		}

		this->DeleteObject(index);
	}

	void OpenWorld::DeleteObject(int objectIndex)
	{
		this->objects[objectIndex]->Destroy();
		this->objects.erase(this->objects.begin() + objectIndex);
	}

	OpenObject* OpenWorld::GetObject(int objectIndex)
	{
		return this->objects[objectIndex];
	}

	int OpenWorld::GetObjectCount() 
	{
		return this->objects.size();
	}

	OpenWorld::OpenWorld() 
	{
		worldInstance = this;
		this->objects = std::vector<OpenObject*>();
	}

	void OpenWorld::Update(float delta)
	{
		for (int i = 0; i < this->objects.size(); i++) {
			this->objects[i]->Update(delta);
		}
	}

	void OpenWorld::Destroy()
	{
		for (int i = 0; i < this->objects.size(); i++) {
			this->objects[i]->Destroy();
		}

		delete worldInstance;
		worldInstance = nullptr;
	}

	void OpenObject::AddComponent(Component* component)
	{
		this->components.push_back(component);
	}

	void OpenObject::RemoveComponent(Component* component)
	{
		int index = 0;
		for (index = 0; index < this->components.size(); index++) {
			if (component == this->components[index]) {
				break;
			}
		}

		this->RemoveComponent(index);
	}

	void OpenObject::RemoveComponent(int componentIndex)
	{
		this->components[componentIndex]->Destroy();
		this->components.erase(this->components.begin() + componentIndex);
	}

	Component* OpenObject::GetComponent(int componentIndex)
	{
		return this->components[componentIndex];
	}

	int OpenObject::GetComponentCount() 
	{
		return this->components.size();
	}

	OpenObject::OpenObject(std::string name)
	{
		this->name = name;
		this->Initialize();
	}

	OpenObject::OpenObject() 
	{
		this->name = "";
		this->Initialize();
	}

	void OpenObject::Initialize() 
	{
		worldInstance->AddObject(this);
		this->components = std::vector<Component*>();
		this->transform = CreateComponent<Transform>(this);
	}

	void OpenObject::Update(float delta) 
	{
		for (int i = 0; i < this->GetComponentCount(); i++)
		{
			this->GetComponent(i)->Update(delta);
		}
	}

	void OpenObject::Destroy()
	{
		for (int i = 0; i < this->components.size(); i++) {
			Component* comp = this->GetComponent(i);
			comp->Destroy();
			comp->DestroyInternal();
		}

		delete this;
	}

	Component::Component()
	{
		this->openObject = nullptr;
	}

	void Component::DestroyInternal()
	{
		this->openObject->RemoveComponent(this);
		delete this;
	}

	void Transform::Awake()
	{
		this->position = glm::vec3(0.0f);
		this->eulerRotation = glm::vec3(0.0f);
		this->scale = glm::vec3(1.0f);
	}
}