#pragma once
#include "Core.h"
namespace Mini
{
	class MINI_API Application
	{
	public:
		Application();
		~Application();
		void Run();
	};

	Application* CreateApplication();
}
