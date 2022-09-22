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
		this->selectedObjectId = "";
		this->objectDropdownsOpen = std::map<std::string, bool>();
	}

	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
	ImGuiTreeNodeFlags noChildrenFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	ImGuiTreeNodeFlags hasChildrenFlags = ImGuiTreeNodeFlags_OpenOnArrow;

	void ObjectView::ShouldSelectNode(std::string id, Engine::OpenObject* object, bool wasJustOpen, bool isOpen)
	{
		bool clicked = ImGui::IsItemClicked();
		bool justToggledDropdown = wasJustOpen != isOpen;

		if (clicked && justToggledDropdown) {
			this->selectedObjectId = this->previousSelectedObjectId;
			this->selectedObject = this->previousSelectedObject;
		}

		bool shouldSelect = clicked && !justToggledDropdown;
		if (!shouldSelect)
			return;

		this->selectedObjectId = id;
		this->selectedObject = object;
	}

	bool ObjectView::RegisterTreeNode(std::string id, bool hasChildren, const char* objectName, bool& wasAlreadyOpen)
	{
		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if (this->selectedObjectId == id)
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		nodeFlags |= (hasChildren ? hasChildrenFlags : noChildrenFlags);

		bool openEntryIsInsideMap = hasChildren && this->objectDropdownsOpen.find(id) != this->objectDropdownsOpen.end();
		wasAlreadyOpen = openEntryIsInsideMap && this->objectDropdownsOpen[id];
		bool isOpen = ImGui::TreeNodeEx(&id, nodeFlags, objectName);

		if (hasChildren) 
		{
			if (!openEntryIsInsideMap)
				this->objectDropdownsOpen.insert({ id, isOpen });
			else
				this->objectDropdownsOpen[id] = isOpen;
		}

		return hasChildren ? this->objectDropdownsOpen[id] : false;
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

			std::string id = "ObjectView::ChildObject[" + std::to_string(index) + "][" + std::to_string(j) + "]";
			bool wasAlreadyOpen;
			bool open = this->RegisterTreeNode(id, hasChildren, objectName, wasAlreadyOpen);
			this->ShouldSelectNode(id, targetChild, wasAlreadyOpen, open);

			if (hasChildren && open)
			{
				*targetObject = targetChild;
				childDisplacement = 0;
				
				for (index = 0; index < Engine::WorldInstance->GetObjectCount(); index++)
					if (Engine::WorldInstance->GetObject(index) == targetChild)
						break;

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

			for (index = 0; index < Engine::WorldInstance->GetObjectCount(); index++)
				if (Engine::WorldInstance->GetObject(index) == parent)
					break;

			isRoot = parent->GetParent() == nullptr;
		}

		*targetObject = parent;
	}

	void ObjectView::Render()
	{
		ImGui::Begin("Object View");

		this->previousSelectedObject = this->selectedObject;
		this->previousSelectedObjectId = this->selectedObjectId;

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton(0))) 
		{
			this->selectedObject = nullptr;
			this->selectedObjectId = "";
		}

		std::string rootIdentifier = std::string("RootObject");
		std::string childRootIdentifier = std::string("ChildRootObject");

		bool isRoot = true;
		int index = 0;

		for (int i = 0; i < Engine::WorldInstance->GetObjectCount(); i++)
		{
			Engine::OpenObject* targetObject = Engine::WorldInstance->GetObject(i);
			if (targetObject->GetParent() != nullptr)
				continue;

			isRoot = true;
			index = i;

			const char* objectName = "[Empty Name]";
			if (targetObject->name != "")
				objectName = targetObject->name.c_str();

			std::string id = "ObjectView::" + (isRoot ? rootIdentifier : childRootIdentifier) + "[" + std::to_string(index) + "]";
			bool wasAlreadyOpen;
			bool open = RegisterTreeNode(id, targetObject->GetChildCount() != 0, objectName, wasAlreadyOpen);
			this->ShouldSelectNode(id, targetObject, wasAlreadyOpen, open);

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

		std::cout << window;
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