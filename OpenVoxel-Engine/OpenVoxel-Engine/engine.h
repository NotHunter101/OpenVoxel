#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <iostream>

namespace Engine 
{
	class OpenScene;
	class OpenObject;
	class Component;
	class Transform;

	extern OpenScene* SceneInstance;

	void SetActiveScene(OpenScene* scene);

	template <class T>
	class SharedPtr 
	{
	private:
		T* pointer;
	public:
		SharedPtr(T* pointer) 
		{
			this->pointer = pointer;
		}

		void SetPointer(T* pointer) 
		{
			this->pointer = pointer;
		}

		void Delete() 
		{
			delete this->pointer;
			this->pointer = nullptr;
			delete this;
		}

		bool PointerExists() 
		{
			return pointer != nullptr;
		}

		T* Ptr() const
		{
			if (pointer == nullptr) {
				std::cout << "ERROR: Pointer is being accessed when the underlying memory has been deleted (Are you using a destroyed object?)" << std::endl;
				throw std::runtime_error("");
			}

			return pointer;
		}
	};

	class OpenScene
	{
	private:
		std::vector<SharedPtr<OpenObject>*> objects;
	public:
		SharedPtr<OpenObject>* AddObject(std::string name);
		void DeleteObject(OpenObject* object);
		SharedPtr<OpenObject>* GetObject(int objectIndex);
		int GetObjectCount();

		OpenScene();

		void Update(float delta);
		void Destroy();
	};

	class OpenObject 
	{
	private:
		std::vector<SharedPtr<Component>*> components;
		std::vector<SharedPtr<OpenObject>*> children;
		SharedPtr<OpenObject>* parentPtr;
		OpenObject* parent() {
			if (parentPtr == nullptr)
				return nullptr;
			return parentPtr->Ptr();
		}
		bool destroyed;

	public:
		OpenObject() {}
		void Initialize();

		SharedPtr<Transform>* transformPtr;
		Transform* transform() {
			if (transformPtr == nullptr)
				return nullptr;
			return transformPtr->Ptr();
		}

		std::string name;
		int sceneIndex;

		static void SetParent(SharedPtr<OpenObject>* child, SharedPtr<OpenObject>* parent);
		SharedPtr<OpenObject>* GetParent();
		SharedPtr<OpenObject>* GetChild(int index);
		int GetChildCount();

		void AddComponent(SharedPtr<Component>* component);
		void RemoveComponentInternal(Component* component);
		void RemoveComponent(Component* component);
		SharedPtr<Component>* GetComponent(int componentIndex);
		int GetComponentCount();

		template <class T>
		SharedPtr<T>* GetComponent()
		{
			for (SharedPtr<Component>* component : this->components)
				if (typeid(T).name() == typeid(*component->Ptr()).name())
					return (SharedPtr<T>*)component;
		}

		void Update(float delta);
		void Destroy();
	};

	class Component 
	{
	public:
		SharedPtr<OpenObject>* openObjectPtr;
		OpenObject* openObject() {
			if (openObjectPtr == nullptr)
				return nullptr;
			return openObjectPtr->Ptr();
		}

		SharedPtr<Transform>* transformPtr;
		Transform* transform() {
			if (transformPtr == nullptr)
				return nullptr;
			return transformPtr->Ptr();
		}
		
		void DestroyInternal();
		virtual void Awake() {};
		virtual void Update(float delta) {};
		virtual void Destroy() {};
	};

	class Transform : public Component
	{
	public:
		glm::vec3 position;
		glm::vec3 eulerRotation;
		glm::vec3 scale;

		void Awake() override;
	};

	template<class T>
	SharedPtr<T>* CreateComponent(SharedPtr<OpenObject>* object)
	{
		T* component = new T();
		component->openObjectPtr = object;
		component->transformPtr = object->Ptr()->transformPtr;
		component->Awake();

		SharedPtr<T>* sharedPtr = new SharedPtr<T>(component);
		object->Ptr()->AddComponent((SharedPtr<Component>*)sharedPtr);
		return sharedPtr;
	}
}