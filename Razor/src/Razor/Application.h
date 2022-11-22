#pragma once
#include "Razor/Core.h"
#include "Razor/Window.h"
#include "Razor/Events/Event.h"
#include "Razor/Events/ApplicationEvent.h"
#include "Razor/Events/KeyEvent.h"
#include "Razor/LayerStack.h"


#include <memory>

namespace Razor
{
	class ImGuiLayer;
	class Application
	{
	public:
		Application();
		~Application();
		void Run();
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		inline Window& GetWindow()  { return *m_Window; }
		static inline Application& Get() { return *s_Instance; }
		void OnEvent(Event& e);
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;
	};

	Application* CreateApplication();
}
