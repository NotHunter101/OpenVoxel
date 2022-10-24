#include "engine.h"

#include <sstream>
#include <iostream>
#include <string>

namespace Engine 
{
	OpenScene* SceneInstance;

	void SetActiveScene(OpenScene* scene)
	{
		SceneInstance = scene;
	}

	SharedPtr<OpenObject>* OpenScene::AddObject(std::string name)
	{
		OpenObject* object = new OpenObject();
		object->name = name;
		object->Initialize();
		object->sceneIndex = this->objects.size();

		SharedPtr<OpenObject>* objectPtr = new SharedPtr<OpenObject>(object);
		object->transformPtr = CreateComponent<Transform>(objectPtr);
		this->objects.push_back(objectPtr);
		return objectPtr;
	}

	void OpenScene::DeleteObject(OpenObject* object)
	{
		int sceneIndex = (*object).sceneIndex;
		for (int i = sceneIndex + 1; i < this->objects.size(); i++) {
			this->objects[i]->Ptr()->sceneIndex = i - 1;
		}

		this->objects[sceneIndex]->Ptr()->Destroy();
		this->objects[sceneIndex]->Delete();
		this->objects.erase(this->objects.begin() + sceneIndex);
	}

	SharedPtr<OpenObject>* OpenScene::GetObject(int objectIndex)
	{
		return this->objects[objectIndex];
	}

	int OpenScene::GetObjectCount()
	{
		return this->objects.size();
	}

	OpenScene::OpenScene()
	{
		this->objects = std::vector<SharedPtr<OpenObject>*>();
	}

	void OpenScene::Update(float delta)
	{
		for (int i = 0; i < this->objects.size(); i++) {
			this->objects[i]->Ptr()->Update(delta);
		}
	}

	void OpenScene::Destroy()
	{
		for (int i = 0; i < this->objects.size(); i++) {
			this->objects[i]->Ptr()->Destroy();
		}

		delete SceneInstance;
		SceneInstance = nullptr;
	}

	void OpenObject::Initialize() 
	{
		this->children = std::vector<SharedPtr<OpenObject>*>();
		this->components = std::vector<SharedPtr<Component>*>();
		this->destroyed = false;
		this->parentPtr = nullptr;
	}

	void OpenObject::SetParent(SharedPtr<OpenObject>* child, SharedPtr<OpenObject>* parent)
	{
		if (child->Ptr()->parent() != nullptr) {
			int index = 0;
			for (index = 0; index < child->Ptr()->parent()->GetChildCount(); index++)
				if (child->Ptr()->parent()->GetChild(index) == child)
					break;

			std::vector<SharedPtr<OpenObject>*>* children = &child->Ptr()->parent()->children;
			children->erase(children->begin() + index);
		}

		if (parent != nullptr && !parent->PointerExists())
		{
			std::cout << "WARNING: Do not pass 'new SharedPtr<OpenObject>(nullptr)' as parent; pass 'nullptr' instead.";
			delete parent;
			parent = nullptr;
		}

		child->Ptr()->parentPtr = parent;

		if (parent != nullptr)
			parent->Ptr()->children.push_back(child);
	}

	SharedPtr<OpenObject>* OpenObject::GetParent()
	{
		return this->parentPtr;
	}

	SharedPtr<OpenObject>* OpenObject::GetChild(int index)
	{
		return this->children[index];
	}

	int OpenObject::GetChildCount() 
	{
		return this->children.size();
	}

	void OpenObject::AddComponent(SharedPtr<Component>* component)
	{
		this->components.push_back(component);
	}

	void OpenObject::RemoveComponentInternal(Component* component) 
	{
		int index = 0;
		for (index = 0; index < this->components.size(); index++) {
			if (component == this->components[index]->Ptr()) {
				break;
			}

			if (index == this->components.size() - 1) {
				std::cout << "ERROR: Component to be removed was not found on object" << std::endl;
				return;
			}
		}

		this->components[index]->Ptr()->Destroy();
		this->components[index]->Delete();
		this->components.erase(this->components.begin() + index);
	}

	void OpenObject::RemoveComponent(Component* component)
	{
		if (component == this->transform())
		{
			std::cout << "ERROR: Component to be removed was of 'Transform' type! This is not possible" << std::endl;
			return;
		}

		RemoveComponentInternal(component);
	}

	SharedPtr<Component>* OpenObject::GetComponent(int componentIndex)
	{
		return this->components[componentIndex];
	}

	int OpenObject::GetComponentCount() 
	{
		return this->components.size();
	}

	void OpenObject::Update(float delta) 
	{
		for (int i = 0; i < this->GetComponentCount(); i++)
		{
			this->GetComponent(i)->Ptr()->Update(delta);
		}
	}

	void OpenObject::Destroy()
	{
		if (this->destroyed == true)
			return;

		int initialCompCount = this->components.size();
		for (int i = 0; i < initialCompCount; i++) {
			SharedPtr<Component>* comp = this->GetComponent(0);
			comp->Ptr()->DestroyInternal();
		}

		this->destroyed = true;
		SceneInstance->DeleteObject(this);
	}

	void Component::DestroyInternal()
	{
		this->openObject()->RemoveComponentInternal(this);
	}

	void Transform::Awake()
	{
		this->position = glm::vec3(0.0f);
		this->eulerRotation = glm::vec3(0.0f);
		this->scale = glm::vec3(1.0f);
	}
}