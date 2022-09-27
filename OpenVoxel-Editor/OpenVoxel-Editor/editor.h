#pragma once

#include "engine.h"
#include <map>
#include <typeinfo>

namespace Editor 
{
	class EditorApplication;
	class EditorWindow;
	class ObjectView;

	extern EditorApplication* ApplicationInstance;

	class EditorApplication 
	{
	private:
		std::vector<EditorWindow*> windows;
	public:
		EditorApplication();

		void AddWindow(EditorWindow* window);
		void DeleteWindow(EditorWindow* window);
		EditorWindow* GetWindow(const std::type_info* windowType);
		void RenderWindows();
		void Destroy();
	};
	
	class EditorWindow 
	{
	public:
		virtual void Initialize() {};
		virtual void Render() {};
	};

	class ObjectView : public EditorWindow
	{
	private:
		Engine::OpenObject* oldSelectedObject;
		int oldSelectedObjectIndex;

		Engine::OpenObject* selectedObject;
		int selectedObjectIndex;

		std::map<int, bool> objectDropdownsOpen;

		bool RegisterTreeNode(int objectIndex, bool hasChildren, const char* objectName, bool& wasAlreadyOpen);
		void PopulateChildren(int& index, bool& isRoot, Engine::OpenObject** targetObject, int& childDisplacement);
		void ShouldSelectNode(int objectIndex, Engine::OpenObject* object, bool wasJustOpen, bool isOpen);
	public:
		void Initialize() override;
		void Render() override;
		Engine::OpenObject* GetTargetedObject();
	};

	class ObjectInspector : public EditorWindow
	{
	private:
		ObjectView* objectViewWindow;
	public:
		void Initialize() override;
		void Render() override;
	};

	class ProjectContent : public EditorWindow
	{
	public:
		//void Initialize() override;
		//void Render() override;
	};

	template<class T>
	T* CreateWindow()
	{
		T* window = new T();
		ApplicationInstance->AddWindow(window);
		window->Initialize();

		return window;
	}
}