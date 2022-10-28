#include "rzpch.h"
#include "Application.h"
#include "Razor/Events/ApplicationEvent.h"
#include "Razor/Log.h"
using namespace Razor;

Application::Application()
{
}

Application::~Application()
{
}

void Application::Run()
{
	WindowResizeEvent e(1920, 1080);
	RZ_TRACE(e);
	while (true) 
	{

	};
}
