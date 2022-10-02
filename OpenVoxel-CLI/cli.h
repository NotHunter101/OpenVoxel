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

	public ref class GlfwApplication
	{
	internal:
		int managedWrapperCount;
	public:
		static GlfwApplication^ Instance;
		GlfwApplication();

		void Initialize();
		void Start();

		OpenObject^ CreateObject(String^ name);
	};

	template <class T>
	public ref class ClassWrapper
	{
	internal:
		Engine::SharedPointer<T>* generalPointer;
		bool pointerExternallyManaged;

		property Engine::SharedPointer<T>* instance
		{
			Engine::SharedPointer<T>* get()
			{
				return (Engine::SharedPointer<T>*)generalPointer;
			}
			void set(Engine::SharedPointer<T>* pointer)
			{
				generalPointer = pointer;
			}
		}

	public:
		ClassWrapper() 
		{
			pointerExternallyManaged = false;
			generalPointer = new Engine::SharedPointer<T>(new T());
		}

		ClassWrapper(Engine::SharedPointer<T>* instance)
		{
			pointerExternallyManaged = true;
			this->generalPointer = instance;
		}

		~ClassWrapper()
		{
			if (generalPointer == nullptr)
				return;

			generalPointer->Delete();
			generalPointer = nullptr;
		}

		!ClassWrapper()
		{
			if (generalPointer == nullptr)
				return;

			if (!pointerExternallyManaged)
				generalPointer->Delete();
			generalPointer = nullptr;
		}
	};

	public ref class Vector3 
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

	template <class T>
	public ref class ComponentWrapper : public ClassWrapper<T>
	{
	public:
		OpenObject^ openObject;
		Transform^ transform;
	};

	public ref class Component : public ComponentWrapper<ManagedComponent>
	{
	public:
		Component() {}

		virtual void Awake() {}
		virtual void Update(float delta) {}
		virtual void Destroy() {}
	};

	public ref class Transform : ComponentWrapper<Engine::Transform>
	{
	public:
		property Vector3^ position
		{
		public:
			Vector3^ get()
			{
				return %Vector3::FromGlm(instance->Pointer()->position);
			}
			void set(Vector3^ value)
			{
				instance->Pointer()->position = value->ToGlm();
			}
		}

		property Vector3^ scale
		{
		public:
			Vector3^ get()
			{
				return %Vector3::FromGlm(instance->Pointer()->scale);
			}
			void set(Vector3^ value)
			{
				instance->Pointer()->scale = value->ToGlm();
			}
		}

		property Vector3^ eulerRotation
		{
		public:
			Vector3^ get()
			{
				return %Vector3::FromGlm(instance->Pointer()->eulerRotation);
			}
			void set(Vector3^ value)
			{
				instance->Pointer()->eulerRotation = value->ToGlm();
			}
		}
	};
}