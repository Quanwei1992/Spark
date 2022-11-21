#pragma once
#include "Razor/Core.h"
#include "Razor/Window.h"
#include "Razor/Events/Event.h"
#include "Razor/Events/ApplicationEvent.h"
#include "Razor/Events/KeyEvent.h"
#include "Razor/LayerStack.h"

#include "Razor/Renderer/OrthographicCamera.h"

#include <memory>

namespace Razor
{
	class ImGuiLayer;
	class VertexBuffer;
	class IndexBuffer;
	class VertexArray;
	class Shader;
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

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;

		OrthographicCamera m_Camera;
	};

	Application* CreateApplication();
}
