#pragma once
#include "Spark/Core/Base.h"
#include "Spark/Core/Window.h"
#include "Spark/Events/Event.h"
#include "Spark/Events/ApplicationEvent.h"
#include "Spark/Events/KeyEvent.h"
#include "Spark/Core/LayerStack.h"
#include "Spark/Core/Timestep.h"


#include <memory>

namespace Spark
{

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			SK_CORE_ASSERT(index < Count, "Out of range!");
			return Args[index];
		}
	};


	class ImGuiLayer;
	class Application
	{
	public:
		Application(const std::string& name = "Spark",ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		~Application();
		void Run();
		void Exit();
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		inline Window& GetWindow()  { return *m_Window; }
		static inline Application& Get() { return *s_Instance; }
		void OnEvent(Event& e);
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }
	private:
		void RenderImGui();
	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		ApplicationCommandLineArgs m_CommandLineArgs;
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

	Application* CreateApplication(ApplicationCommandLineArgs args);
}
