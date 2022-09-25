#include "cli.h"
#include "main.h"

#include <vector>
#include <iostream>

namespace CLI
{
	GlfwApplication::GlfwApplication()
	{
		testArrayCount = 0;
		testArray = gcnew array<ComponentTest^>(100);
		ComponentTest^ test = gcnew ComponentTest();
		this->AddComponent(test);
	}

	void GlfwApplication::Start()
	{
		createGLFW();
	}

	void GlfwApplication::TestInheritance()
	{
		for (int i = 0; i < testArrayCount; i++)
		{
			testArray[i]->Test();
		}
	}

	void GlfwApplication::AddComponent(ComponentTest^ test) 
	{
		testArray[testArrayCount] = test;
		testArrayCount++;
	}

	void ComponentTest::Test()
	{
		std::cout << "Foo\n";
	}
}