#pragma once

#include "engine.h"
#include <iostream>
#include "gcroot.h"

using namespace System;

namespace CLI
{
	ref class Vector3;
	ref class OpenObject;
	class ManagedComponent;
	ref class Component;
	ref class Transform;
	ref class GlfwApplication;

	template <class T>
	public ref class ClassWrapper
	{
	internal:
		T* instance;

	public:
		ClassWrapper() 
		{
			instance = new T();
		}

		ClassWrapper(T* instance)
		{
			this->instance = instance;
		}

		~ClassWrapper()
		{
			delete instance;
		}

		!ClassWrapper()
		{
			delete instance;
		}
	};

	public ref struct Vector3 
	{
	internal:
		static Vector3 FromGlm(const glm::vec3 vec);
		glm::vec3 ToGlm();
		
	public:
		Vector3(const Vector3% copy);
		Vector3() {};
		Vector3(float x, float y, float z);

		float x;
		float y;
		float z;
	};

	public ref class OpenObject : public ClassWrapper<Engine::OpenObject>
	{
	public:
		OpenObject(String^ name);
		Transform^ transform;

		void AddComponent(Component^ component);
	};

	private class ManagedComponent : public Engine::Component
	{
	protected:
		bool pointerSet;
		gcroot<CLI::Component^> managedComponentPointer;
	public:
		ManagedComponent(CLI::Component^ managedComponent);
		ManagedComponent();

		void Awake() override;
		void Update(float delta) override;
		void Destroy() override;
	};

	private interface struct IComponent 
	{
	public:
		property OpenObject^ openObject;
		property Transform^ transform;
	};

	template <class T>
	public ref class ComponentWrapper : public IComponent, public ClassWrapper<T>
	{
	protected:
		OpenObject^ _openObject;
		Transform^ _transform;

	public:
		property OpenObject^ openObject
		{
			virtual OpenObject^ get() { return _openObject; }
			virtual void set(OpenObject^ value) { _openObject = value; }
		}

		property Transform^ transform
		{
			virtual Transform^ get() { return _transform; }
			virtual void set(Transform^ value);
		}
	};

	public ref class Component : public ComponentWrapper<ManagedComponent>
	{
	public:
		Component();

		virtual void Awake() {}
		virtual void Update(float delta) {}
		virtual void Destroy() {}
	};

	public ref class Transform : ComponentWrapper<Engine::Transform>
	{
	public:
		property Vector3 postion
		{
		public:
			Vector3 get()
			{
				return Vector3::FromGlm(instance->position);
			}
			void set(Vector3 value)
			{
				instance->position = value.ToGlm();
			}
		}

		property Vector3 scale
		{
		public:
			Vector3 get()
			{
				return Vector3::FromGlm(instance->scale);
			}
			void set(Vector3 value)
			{
				instance->scale = value.ToGlm();
			}
		}

		property Vector3 eulerRotation
		{
		public:
			Vector3 get()
			{
				return Vector3::FromGlm(instance->eulerRotation);
			}
			void set(Vector3 value)
			{
				instance->eulerRotation = value.ToGlm();
			}
		}
	};

	public ref class GlfwApplication
	{
	public:
		static GlfwApplication^ Instance;
		GlfwApplication();

		void Initialize();
		void Start();
	};
}