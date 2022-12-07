#include "skpch.h"
#include "Application.h"
#include "Spark/Core/Log.h"
#include "Spark/Events/Input.h"
#include "Spark/Core/KeyCodes.h"
#include "Spark/ImGui/ImGuiLayer.h"
#include "Spark/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace Spark
{

	Application* Application::s_Instance = nullptr;

	Application::Application()	
	{
		SK_PROFILE_FUNCTION();
		SK_CORE_ASSERT(!s_Instance, "Application already exsits!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(SK_BIND_EVENT_FN(Application::OnEvent));
		Renderer::Init();
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);	
	}

	void Application::OnEvent(Event& e)
	{
		SK_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(SK_BIND_EVENT_FN(Application::OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(SK_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled) break;
		}
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		SK_PROFILE_FUNCTION();
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::OnWindowResized(e.GetWidth(), e.GetHeight());
		return false;
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		SK_PROFILE_FUNCTION();
		while (m_Running)
		{
			SK_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime(); // Platform::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				SK_PROFILE_SCOPE("LayerStack OnUpdate");
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(timestep);
				}
			}
			{
				SK_PROFILE_SCOPE("LayerStack OnImGuiUpdate");
				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
				{
					layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		};
	}

	void Application::Exit()
	{
		m_Running = false;
	}

	void Application::PushLayer(Layer* layer)
	{
		SK_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		SK_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}


}

