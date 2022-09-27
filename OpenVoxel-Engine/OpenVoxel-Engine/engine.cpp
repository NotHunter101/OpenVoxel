#include "engine.h"

#include <sstream>
#include <iostream>
#include <string>

namespace Engine 
{
	OpenScene* SceneInstance;
	void (*DeleteHandleFromSharpHeap)(void*);

	void SetActiveScene(OpenScene* scene) 
	{
		SceneInstance = scene;
	}	
	
	void SetSharpHeapManagerFunc(void (*function)(void*))
	{
		DeleteHandleFromSharpHeap = function;
	}

	void OpenScene::AddObject(OpenObject* object)
	{
		object->sceneIndex = this->objects.size();
		this->objects.push_back(object);
	}

	void OpenScene::DeleteObject(OpenObject* object)
	{
		this->DeleteObject(object->sceneIndex);
	}

	void OpenScene::DeleteObject(int objectIndex)
	{
		for (int i = objectIndex + 1; i < this->objects.size(); i++)
			this->objects[i]->sceneIndex = i - 1;

		this->objects[objectIndex]->Destroy();
		this->objects.erase(this->objects.begin() + objectIndex);
	}

	OpenObject* OpenScene::GetObject(int objectIndex)
	{
		return this->objects[objectIndex];
	}

	int OpenScene::GetObjectCount()
	{
		return this->objects.size();
	}

	OpenScene::OpenScene()
	{
		this->objects = std::vector<OpenObject*>();
	}

	void OpenScene::Update(float delta)
	{
		for (int i = 0; i < this->objects.size(); i++) {
			this->objects[i]->Update(delta);
		}
	}

	void OpenScene::Destroy()
	{
		for (int i = 0; i < this->objects.size(); i++) {
			this->objects[i]->Destroy();
		}

		delete SceneInstance;
		SceneInstance = nullptr;
	}

	void OpenObject::SetParent(OpenObject* parent)
	{
		if (this->parent != nullptr) {
			int index = 0;
			for (index = 0; index < this->parent->GetChildCount(); index++)
				if (this->parent->GetChild(index) == this)
					break;
			this->parent->children.erase(this->parent->children.begin() + index);
		}

		this->parent = parent;
		if (parent != nullptr)
			parent->children.push_back(this);
	}

	OpenObject* OpenObject::GetParent()
	{
		return this->parent;
	}

	OpenObject* OpenObject::GetChild(int index)
	{
		return this->children[index];
	}

	int OpenObject::GetChildCount() 
	{
		return this->children.size();
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
		this->destroyed = false;
		this->name = name;
		this->Initialize();
	}

	OpenObject::OpenObject() 
	{
		this->destroyed = false;
		this->name = "";
		this->Initialize();
	}

	void OpenObject::Initialize() 
	{
		SceneInstance->AddObject(this);
		this->components = std::vector<Component*>();
		this->children = std::vector<OpenObject*>();
		this->parent = nullptr;
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
		if (this->destroyed == true)
			return;

		int initialCompCount = this->components.size();
		for (int i = 0; i < initialCompCount; i++) {
			Component* comp = this->GetComponent(0);
			comp->DestroyInternal();
		}

		this->destroyed = true;
		SceneInstance->DeleteObject(this);

		if (DeleteHandleFromSharpHeap != nullptr)
			DeleteHandleFromSharpHeap(this);
		delete this;
	}

	void Component::DestroyInternal()
	{
		this->openObject->RemoveComponent(this);
		
		if (DeleteHandleFromSharpHeap != nullptr)
			DeleteHandleFromSharpHeap(this);
		delete this;
	}

	void Transform::Awake()
	{
		this->position = glm::vec3(0.0f);
		this->eulerRotation = glm::vec3(0.0f);
		this->scale = glm::vec3(1.0f);
	}
}