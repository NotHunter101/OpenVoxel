#include "serializer.h"
#include "ImGUI/imgui.h"

#include <map>
#include <typeindex>

namespace Serializer 
{
	void DrawObjectGUI(Engine::OpenObject* object)
	{
		const char* originalNameBuffer = object->name.c_str() + '\0';
		char* nameBuffer = new char[512];
		memcpy(nameBuffer, originalNameBuffer, (object->name.size() + 1) * sizeof(char));

		ImGui::InputText("Object Name", nameBuffer, 512 * sizeof(char));

		if (nameBuffer != (char*)object->name.c_str())
			object->name = (std::string)nameBuffer;

		delete[] nameBuffer;
	}

	void DrawComponentGUI(Engine::Component* component)
	{
		ImGui::Text(typeid(*component).name());
	}
}