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
	class SharedPointer 
	{
	private:
		T* pointer;
	public:
		SharedPointer(T* pointer) 
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

		T* Pointer() const
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
		std::vector<SharedPointer<OpenObject>*> objects;
	public:
		SharedPointer<OpenObject>* AddObject(std::string name);
		void DeleteObject(OpenObject* object);
		SharedPointer<OpenObject>* GetObject(int objectIndex);
		int GetObjectCount();

		OpenScene();

		void Update(float delta);
		void Destroy();
	};

	class OpenObject 
	{
	private:
		std::vector<SharedPointer<Component>*> components;
		std::vector<SharedPointer<OpenObject>*> children;
		SharedPointer<OpenObject>* parentPtr;
		OpenObject* parent() {
			if (parentPtr == nullptr)
				return nullptr;
			return parentPtr->Pointer();
		}
		bool destroyed;

	public:
		OpenObject() {}
		void Initialize();

		SharedPointer<Transform>* transformPtr;
		Transform* transform() {
			if (transformPtr == nullptr)
				return nullptr;
			return transformPtr->Pointer();
		}

		std::string name;
		int sceneIndex;

		static void SetParent(SharedPointer<OpenObject>* child, SharedPointer<OpenObject>* parent);
		SharedPointer<OpenObject>* GetParent();
		SharedPointer<OpenObject>* GetChild(int index);
		int GetChildCount();

		void AddComponent(SharedPointer<Component>* component);
		void RemoveComponentInternal(Component* component);
		void RemoveComponent(Component* component);
		SharedPointer<Component>* GetComponent(int componentIndex);
		int GetComponentCount();

		template <class T>
		SharedPointer<T>* GetComponent()
		{
			for (SharedPointer<Component>* component : this->components)
				if (typeid(T).name() == typeid(*component->Pointer()).name())
					return (SharedPointer<T>*)component;
		}

		void Update(float delta);
		void Destroy();
	};

	class Component 
	{
	public:
		SharedPointer<OpenObject>* openObjectPtr;
		OpenObject* openObject() {
			if (openObjectPtr == nullptr)
				return nullptr;
			return openObjectPtr->Pointer();
		}

		SharedPointer<Transform>* transformPtr;
		Transform* transform() {
			if (transformPtr == nullptr)
				return nullptr;
			return transformPtr->Pointer();
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
	SharedPointer<T>* CreateComponent(SharedPointer<OpenObject>* object)
	{
		T* component = new T();
		component->openObjectPtr = object;
		component->transformPtr = object->Pointer()->transformPtr;
		component->Awake();

		SharedPointer<T>* sharedPointer = new SharedPointer<T>(component);
		object->Pointer()->AddComponent((SharedPointer<Component>*)sharedPointer);
		return sharedPointer;
	}
}