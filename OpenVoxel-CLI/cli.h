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

	public ref class GenericClassWrapper
	{ 
	internal:
		void* generalPointer;
		bool pointerExternallyManaged;
	};

	public ref class GlfwApplication
	{
	internal:
		int managedWrapperCount;
		array<GenericClassWrapper^>^ managedWrappers;
	public:
		static GlfwApplication^ Instance;
		GlfwApplication();

		void Initialize();
		void Start();

		void AddManagedWrapper(GenericClassWrapper^ wrapper);
		void RemoveManagedWrapper(GenericClassWrapper^ wrapper);
	};

	template <class T>
	public ref class ClassWrapper : public GenericClassWrapper
	{
	internal:
		property T* instance 
		{
			T* get()
			{
				return (T*)generalPointer;
			}
			void set(T* pointer)
			{
				generalPointer = pointer;
			}
		}

	public:
		ClassWrapper() 
		{
			pointerExternallyManaged = false;
			generalPointer = new T();
			GlfwApplication::Instance->AddManagedWrapper(this);
		}

		ClassWrapper(T* instance)
		{
			pointerExternallyManaged = true;
			this->generalPointer = instance;
			GlfwApplication::Instance->AddManagedWrapper(this);
		}

		~ClassWrapper()
		{
			if (generalPointer == nullptr)
				return;

			delete generalPointer;
			generalPointer = nullptr;
			GlfwApplication::Instance->RemoveManagedWrapper(this);
		}

		!ClassWrapper()
		{
			if (generalPointer == nullptr)
				return;

			if (!pointerExternallyManaged)
				delete generalPointer;

			generalPointer = nullptr;
			GlfwApplication::Instance->RemoveManagedWrapper(this);
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
	internal:
		OpenObject(String^ name);

	public:
		Transform^ transform;
		void AddComponent(Component^ component);
	};

	public ref class SmartOpenObject 
	{
	private:
		OpenObject^ internalObject;
	public:
		SmartOpenObject(String^ name)
		{
			internalObject = gcnew OpenObject(name);
		}

		property OpenObject^ obj {
			OpenObject^ get() 
			{
				if (internalObject->generalPointer == nullptr)
					return nullptr;

				return internalObject;
			}

			void set(OpenObject^ object)
			{
				std::cout << "ERROR::Cannot set 'obj' property of SmartOpenObject" << std::endl;
			}
		};
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
		property Vector3^ position
		{
		public:
			Vector3^ get()
			{
				return %Vector3::FromGlm(instance->position);
			}
			void set(Vector3^ value)
			{
				instance->position = value->ToGlm();
			}
		}

		property Vector3^ scale
		{
		public:
			Vector3^ get()
			{
				return %Vector3::FromGlm(instance->scale);
			}
			void set(Vector3^ value)
			{
				instance->scale = value->ToGlm();
			}
		}

		property Vector3^ eulerRotation
		{
		public:
			Vector3^ get()
			{
				return %Vector3::FromGlm(instance->eulerRotation);
			}
			void set(Vector3^ value)
			{
				instance->eulerRotation = value->ToGlm();
			}
		}
	};
}