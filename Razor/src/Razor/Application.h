#pragma once
#include "Razor/Core.h"
#include "Razor/Window.h"
#include "Razor/Events/Event.h"
#include "Razor/Events/ApplicationEvent.h"
#include "Razor/LayerStack.h"

namespace Razor
{
	class RAZOR_API Application
	{
	public:
		Application();
		~Application();
		void Run();
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	private:
		void OnEvent(Event& e);
		bool OnWindowClosed(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;

	};

	Application* CreateApplication();
}
