#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"
namespace Razor
{
	class RAZOR_API Application
	{
	public:
		Application();
		~Application();
		void Run();

	private:
		void OnEvent(Event& e);
		bool OnWindowClosed(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;

	};

	Application* CreateApplication();
}
