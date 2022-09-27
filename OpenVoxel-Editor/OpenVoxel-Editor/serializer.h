#pragma once
#include <engine.h>

namespace Serializer
{
	void DrawObjectGUI(Engine::OpenObject* object);
	void DrawComponentGUI(Engine::Component* component);

	/*template <>
	void DrawComponentGUI<Engine::Transform>();

	std::istream& operator>>(std::istream& is, Engine::OpenObject& obj);
	std::ostream& operator<<(std::ostream& os, const Engine::OpenObject& obj);

	std::istream& operator>>(std::istream& is, Engine::Component& obj);
	std::ostream& operator<<(std::ostream& os, const Engine::Component& obj);

	std::ostream& operator>>(std::istream& is, Engine::OpenScene& obj);
	std::ostream& operator<<(std::ostream& os, const Engine::OpenScene& obj);*/
}