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

	SharedPointer<OpenObject>* OpenScene::AddObject(std::string name)
	{
		OpenObject* object = new OpenObject();
		object->name = name;
		object->Initialize();
		object->sceneIndex = this->objects.size();

		SharedPointer<OpenObject>* objectPtr = new SharedPointer<OpenObject>(object);
		object->transformPtr = CreateComponent<Transform>(objectPtr);
		this->objects.push_back(objectPtr);
		return objectPtr;
	}

	void OpenScene::DeleteObject(OpenObject* object)
	{
		int sceneIndex = (*object).sceneIndex;
		for (int i = sceneIndex + 1; i < this->objects.size(); i++) {
			this->objects[i]->Pointer()->sceneIndex = i - 1;
		}

		this->objects[sceneIndex]->Pointer()->Destroy();
		this->objects[sceneIndex]->Delete();
		this->objects.erase(this->objects.begin() + sceneIndex);
	}

	SharedPointer<OpenObject>* OpenScene::GetObject(int objectIndex)
	{
		return this->objects[objectIndex];
	}

	int OpenScene::GetObjectCount()
	{
		return this->objects.size();
	}

	OpenScene::OpenScene()
	{
		this->objects = std::vector<SharedPointer<OpenObject>*>();
	}

	void OpenScene::Update(float delta)
	{
		for (int i = 0; i < this->objects.size(); i++) {
			this->objects[i]->Pointer()->Update(delta);
		}
	}

	void OpenScene::Destroy()
	{
		for (int i = 0; i < this->objects.size(); i++) {
			this->objects[i]->Pointer()->Destroy();
		}

		delete SceneInstance;
		SceneInstance = nullptr;
	}

	void OpenObject::Initialize() 
	{
		this->children = std::vector<SharedPointer<OpenObject>*>();
		this->components = std::vector<SharedPointer<Component>*>();
		this->destroyed = false;
		this->parentPtr = nullptr;
	}

	void OpenObject::SetParent(SharedPointer<OpenObject>* child, SharedPointer<OpenObject>* parent)
	{
		if (child->Pointer()->parent() != nullptr) {
			int index = 0;
			for (index = 0; index < child->Pointer()->parent()->GetChildCount(); index++)
				if (child->Pointer()->parent()->GetChild(index) == child)
					break;

			std::vector<SharedPointer<OpenObject>*>* children = &child->Pointer()->parent()->children;
			children->erase(children->begin() + index);
		}

		if (parent != nullptr && !parent->PointerExists())
		{
			std::cout << "WARNING: Do not pass 'new SharedPointer<OpenObject>(nullptr)' as parent; pass 'nullptr' instead.";
			delete parent;
			parent = nullptr;
		}

		child->Pointer()->parentPtr = parent;

		if (parent != nullptr)
			parent->Pointer()->children.push_back(child);
	}

	SharedPointer<OpenObject>* OpenObject::GetParent()
	{
		return this->parentPtr;
	}

	SharedPointer<OpenObject>* OpenObject::GetChild(int index)
	{
		return this->children[index];
	}

	int OpenObject::GetChildCount() 
	{
		return this->children.size();
	}

	void OpenObject::AddComponent(SharedPointer<Component>* component)
	{
		this->components.push_back(component);
	}

	void OpenObject::RemoveComponentInternal(Component* component) 
	{
		int index = 0;
		for (index = 0; index < this->components.size(); index++) {
			if (component == this->components[index]->Pointer()) {
				break;
			}

			if (index == this->components.size() - 1) {
				std::cout << "ERROR: Component to be removed was not found on object" << std::endl;
				return;
			}
		}

		this->components[index]->Pointer()->Destroy();
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

	SharedPointer<Component>* OpenObject::GetComponent(int componentIndex)
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
			this->GetComponent(i)->Pointer()->Update(delta);
		}
	}

	void OpenObject::Destroy()
	{
		if (this->destroyed == true)
			return;

		int initialCompCount = this->components.size();
		for (int i = 0; i < initialCompCount; i++) {
			SharedPointer<Component>* comp = this->GetComponent(0);
			comp->Pointer()->DestroyInternal();
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