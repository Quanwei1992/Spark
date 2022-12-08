#pragma once
#include "Spark/Core/Core.h"
#include "Spark/Core/Window.h"
#include "Spark/Events/Event.h"
#include "Spark/Events/ApplicationEvent.h"
#include "Spark/Events/KeyEvent.h"
#include "Spark/Core/LayerStack.h"
#include "Spark/Core/Timestep.h"


#include <memory>

namespace Spark
{
	class ImGuiLayer;
	class Application
	{
	public:
		Application(const std::string& name = "Spark");
		~Application();
		void Run();
		void Exit();
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
		std::string m_Name;
	};

	Application* CreateApplication();
}
