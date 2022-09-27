#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <typeinfo>

namespace Engine 
{
	class OpenScene;
	class OpenObject;
	class Component;
	class Transform;

	extern OpenScene* SceneInstance;

	void SetActiveScene(OpenScene* scene);
	void SetSharpHeapManagerFunc(void (*function)(void*));

	class OpenScene
	{
	private:
		std::vector<OpenObject*> objects;
	public:
		void AddObject(OpenObject* object);
		void DeleteObject(OpenObject* object);
		void DeleteObject(int objectIndex);
		OpenObject* GetObject(int objectIndex);
		int GetObjectCount();

		OpenScene();

		void Update(float delta);
		void Destroy();
	};

	class OpenObject 
	{
	private:
		std::vector<Component*> components;
		std::vector<OpenObject*> children;
		OpenObject* parent;
		bool destroyed;

		void Initialize();
	public:
		Transform* transform;
		std::string name;
		int sceneIndex;

		void SetParent(OpenObject* parent);
		OpenObject* GetParent();
		OpenObject* GetChild(int index);
		int GetChildCount();

		void AddComponent(Component* component);
		void RemoveComponent(Component* component);
		void RemoveComponent(int componentIndex);
		Component* GetComponent(int componentIndex);
		int GetComponentCount();

		template <class T>
		T* GetComponent()
		{
			for (Component* component : this->components)
				if (typeid(T).name() == typeid(*component).name())
					return (T*)component;
		}

		OpenObject(std::string name);
		OpenObject();

		void Update(float delta);
		void Destroy();
	};

	class Component 
	{
	public:
		OpenObject* openObject;
		Transform* transform;
		
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
	T* CreateComponent(OpenObject* object)
	{
		T* component = new T();
		component->openObject = object;
		component->transform = object->transform;
		object->AddComponent(component);
		component->Awake();

		return component;
	}
}