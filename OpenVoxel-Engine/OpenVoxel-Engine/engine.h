#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

namespace Engine 
{
	class OpenWorld;
	class OpenObject;
	class Component;
	class Transform;

	class OpenWorld 
	{
	private:
		std::vector<OpenObject*> objects;
	public:
		void AddObject(OpenObject* object);
		void DeleteObject(OpenObject* object);
		void DeleteObject(int objectIndex);
		OpenObject* GetObject(int objectIndex);
		int GetObjectCount();

		OpenWorld();

		void Update(float delta);
		void Destroy();
	};

	class OpenObject 
	{
	private:
		std::vector<Component*> components;

		void Initialize();
	public:
		Transform* transform;
		std::string name;
		
		void AddComponent(Component* component);
		void RemoveComponent(Component* component);
		void RemoveComponent(int componentIndex);
		Component* GetComponent(int componentIndex);
		int GetComponentCount();

		OpenObject(std::string name);
		OpenObject();

		void Update(float delta);
		void Destroy();
	};

	class Component 
	{
	public:
		OpenObject* openObject;
		
		Component();

		void DestroyInternal();
		virtual void Awake() {};
		virtual void Update(float delta) {};
		virtual void Destroy() {};
	};

	class Transform : public Component
	{
	public:
		using Component::Component;

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
		object->AddComponent(component);
		component->Awake();

		return component;
	}
}