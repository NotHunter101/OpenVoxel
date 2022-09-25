#pragma once
using namespace System;

namespace CLI
{
	public ref class ComponentTest
	{
	public:
		virtual void Test();
	};

	public ref class GlfwApplication
	{
	private:
		int testArrayCount;
		array<ComponentTest^>^ testArray;
	public:
		GlfwApplication();

		void Start();
		void TestInheritance();
		void AddComponent(ComponentTest^ test);
	};
}
