#include "cli.h"
#include "main.h"

#include <vector>

namespace CLI
{
	using namespace System::Runtime::InteropServices;
	static std::string stringToStdString(String^ string)
	{
		const char* str = (const char*)(Marshal::StringToHGlobalAnsi(string)).ToPointer();
		return std::string(str);
	}

	Vector3 Vector3::FromGlm(const glm::vec3 vec)
	{
		Vector3 managedVec(vec.x, vec.y, vec.z);
		return managedVec;
	}

	glm::vec3 Vector3::ToGlm()
	{
		glm::vec3 vec;
		vec.x = this->x;
		vec.y = this->y;
		vec.z = this->z;
		return vec;
	}

	Vector3::Vector3(const Vector3% copy)
	{
		this->x = copy.x;
		this->y = copy.y;
		this->z = copy.z;
	}

	Vector3::Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	OpenObject::OpenObject(String^ name)
		: ClassWrapper(Engine::SceneInstance->AddObject(stringToStdString(name)))
	{
		Transform^ transform = gcnew Transform();
		transform->instance = this->instance->Pointer()->transformPtr;
		transform->pointerExternallyManaged = true;
		this->transform = transform;
	}

	void OpenObject::AddComponent(Component^ component)
	{
		this->instance->Pointer()->AddComponent((Engine::SharedPointer<Engine::Component>*)component->instance);
	}

	ManagedComponent::ManagedComponent(CLI::Component^ managedComponent)
	{
		pointerSet = true;
		this->managedComponentPointer = managedComponent;
	}	
	
	ManagedComponent::ManagedComponent()
	{
		pointerSet = false;
		this->managedComponentPointer = nullptr;
	}

	void ManagedComponent::Awake()
	{ 
		if (!pointerSet)
			return;

		managedComponentPointer->Awake(); 
	}

	void ManagedComponent::Update(float delta)
	{
		if (!pointerSet)
			return;

		managedComponentPointer->Update(delta);
	}

	void ManagedComponent::Destroy()
	{
		if (!pointerSet)
			return;

		managedComponentPointer->Destroy();
	}

	GlfwApplication::GlfwApplication()
	{
		Instance = this;
	}

	void GlfwApplication::Initialize()
	{
		Game::InitializeGameAndWorld();
	}

	void GlfwApplication::Start()
	{
		Game::StartGameLoop();
	}

	OpenObject^ GlfwApplication::CreateObject(String^ name) 
	{
		OpenObject^ object = gcnew OpenObject(name);
		return object;
	}
}