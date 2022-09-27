#include "editor.h"
#include "serializer.h"
#include "ImGUI/imgui.h"
#include <iostream>

namespace Editor 
{
	EditorApplication* ApplicationInstance;
	
	EditorApplication::EditorApplication()
	{
		this->windows = std::vector<EditorWindow*>();
		ApplicationInstance = this;
	}

	void EditorApplication::AddWindow(EditorWindow* window)
	{
		this->windows.push_back(window);
	}

	void EditorApplication::DeleteWindow(EditorWindow* window)
	{
		int index = 0;
		for (index = 0; index < this->windows.size(); index++)
			if (this->windows[index] == window)
				break;

		this->windows.erase(this->windows.begin() + index);
		delete window;
	}

	EditorWindow* EditorApplication::GetWindow(const std::type_info* windowType)
	{
		for (int i = 0; i < windows.size(); i++)
			if (windowType == &typeid(*windows[i]))
				return windows[i];
	}

	void EditorApplication::RenderWindows()
	{
		for (EditorWindow* window : this->windows)
			window->Render();
	}

	void EditorApplication::Destroy()
	{
		for (EditorWindow* window : this->windows)
			delete window;

		delete this;
	}

	void ObjectView::Initialize()
	{
		this->selectedObject = nullptr;
		this->selectedObjectIndex = 0;
		this->oldSelectedObject = nullptr;
		this->oldSelectedObjectIndex = 0;
		this->objectDropdownsOpen = std::map<int, bool>();
	}

	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
	ImGuiTreeNodeFlags noChildrenFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	ImGuiTreeNodeFlags hasChildrenFlags = ImGuiTreeNodeFlags_OpenOnArrow;

	void ObjectView::ShouldSelectNode(int objectIndex, Engine::OpenObject* object, bool wasJustOpen, bool isOpen)
	{
		bool clicked = ImGui::IsItemClicked();
		bool justToggledDropdown = wasJustOpen != isOpen;

		if (clicked && justToggledDropdown) {
			this->selectedObjectIndex = this->oldSelectedObjectIndex;
			this->selectedObject = this->oldSelectedObject;
		}

		bool shouldSelect = clicked && !justToggledDropdown;
		if (!shouldSelect)
			return;

		this->selectedObjectIndex = objectIndex;
		this->selectedObject = object;
	}

	bool ObjectView::RegisterTreeNode(int objectIndex, bool hasChildren, const char* objectName, bool& wasAlreadyOpen)
	{
		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if (this->selectedObjectIndex == objectIndex)
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		nodeFlags |= (hasChildren ? hasChildrenFlags : noChildrenFlags);

		bool openEntryIsInsideMap = hasChildren && this->objectDropdownsOpen.find(objectIndex) != this->objectDropdownsOpen.end();
		wasAlreadyOpen = openEntryIsInsideMap && this->objectDropdownsOpen[objectIndex];
		bool isOpen = ImGui::TreeNodeEx((void*)(intptr_t)objectIndex, nodeFlags, objectName);

		if (hasChildren) 
		{
			if (!openEntryIsInsideMap)
				this->objectDropdownsOpen.insert({ objectIndex, isOpen });
			else
				this->objectDropdownsOpen[objectIndex] = isOpen;
		}

		return hasChildren ? this->objectDropdownsOpen[objectIndex] : false;
	}

	void ObjectView::PopulateChildren(int& index, bool& isRoot, Engine::OpenObject** targetObject, int& childDisplacement)
	{
		Engine::OpenObject* target = *targetObject;
		 
		for (int j = childDisplacement; j < target->GetChildCount(); j++)
		{
			Engine::OpenObject* targetChild = target->GetChild(j);
			bool hasChildren = targetChild->GetChildCount() != 0;

			const char* objectName = "[Empty Name]";
			if (targetChild->name != "")
				objectName = targetChild->name.c_str();

			bool wasAlreadyOpen;
			bool open = this->RegisterTreeNode(targetChild->sceneIndex, hasChildren, objectName, wasAlreadyOpen);
			this->ShouldSelectNode(targetChild->sceneIndex, targetChild, wasAlreadyOpen, open);

			if (hasChildren && open)
			{
				*targetObject = targetChild;
				childDisplacement = 0;
				index = targetChild->sceneIndex;

				isRoot = false;
				return;
			}
		}

		ImGui::TreePop();

		Engine::OpenObject* parent = target->GetParent();
		if (parent != nullptr)
		{
			for (childDisplacement = 0; childDisplacement < parent->GetChildCount(); childDisplacement++)
				if (parent->GetChild(childDisplacement) == target)
					break;
			childDisplacement++;

			for (index = 0; index < Engine::SceneInstance->GetObjectCount(); index++)
				if (Engine::SceneInstance->GetObject(index) == parent)
					break;

			isRoot = parent->GetParent() == nullptr;
		}

		*targetObject = parent;
	}

	void ObjectView::Render()
	{
		ImGui::Begin("Object View");

		this->oldSelectedObject = this->selectedObject;
		this->oldSelectedObjectIndex = this->selectedObjectIndex;

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton(0))) 
		{
			this->selectedObject = nullptr;
			this->selectedObjectIndex = 0;
		}

		bool isRoot = true;
		int index = 0;

		for (int i = 0; i < Engine::SceneInstance->GetObjectCount(); i++)
		{
			Engine::OpenObject* targetObject = Engine::SceneInstance->GetObject(i);
			if (targetObject->GetParent() != nullptr)
				continue;

			isRoot = true;
			index = i;

			const char* objectName = "[Empty Name]";
			if (targetObject->name != "")
				objectName = targetObject->name.c_str();

			bool wasAlreadyOpen;
			bool open = RegisterTreeNode(targetObject->sceneIndex, targetObject->GetChildCount() != 0, objectName, wasAlreadyOpen);
			this->ShouldSelectNode(targetObject->sceneIndex, targetObject, wasAlreadyOpen, open);

			if (open)
			{
				int childDisplacement = 0;
				while (targetObject != nullptr)
					PopulateChildren(index, isRoot, &targetObject, childDisplacement);
			}
		}
		
		ImGui::End();
	}

	Engine::OpenObject* ObjectView::GetTargetedObject()
	{
		return this->selectedObject;
	}

	void ObjectInspector::Initialize()
	{
		const std::type_info* info = &typeid(ObjectView);
		EditorWindow* window = ApplicationInstance->GetWindow(info);
		this->objectViewWindow = (ObjectView*)window;
	}

	void ObjectInspector::Render()
	{
		ImGui::Begin("Object Inspector");

		Engine::OpenObject* object = this->objectViewWindow->GetTargetedObject();
		if (object != nullptr)
		{
			Serializer::DrawObjectGUI(object);
			for (int i = 0; i < object->GetComponentCount(); i++) 
				Serializer::DrawComponentGUI(object->GetComponent(i));
		}

		ImGui::End();
	}
}