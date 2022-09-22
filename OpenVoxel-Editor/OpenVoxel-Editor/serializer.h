#pragma once
#include <engine.h>

namespace Serializer
{
	void DrawObjectGUI(Engine::OpenObject* object);
	void DrawComponentGUI(Engine::Component* component);

	/*template <>
	void DrawComponentGUI<Engine::Transform>();*/
}