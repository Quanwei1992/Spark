#pragma once
#include "Razor/Core/Core.h"
#include "Razor/Core/Window.h"
#include "Razor/Events/Event.h"
#include "Razor/Events/ApplicationEvent.h"
#include "Razor/Events/KeyEvent.h"
#include "Razor/Core/LayerStack.h"
#include "Razor/Core/Timestep.h"


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
	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Timestep m_Timestep;
		bool m_Minimized = false;
		float m_LastFrameTime = 0;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;
	};

	Application* CreateApplication();
}
