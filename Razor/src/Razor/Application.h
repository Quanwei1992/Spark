#pragma once
#include "Core.h"
namespace Razor
{
	class RAZOR_API Application
	{
	public:
		Application();
		~Application();
		void Run();
	};

	Application* CreateApplication();
}
