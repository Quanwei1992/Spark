#include "rzpch.h"
#include "Application.h"
#include "Razor/Core/Log.h"
#include "Razor/Events/Input.h"
#include "Razor/Core/KeyCodes.h"
#include "Razor/ImGui/ImGuiLayer.h"
#include "Razor/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace Razor
{

	Application* Application::s_Instance = nullptr;

	Application::Application()	
	{
		RZ_PROFILE_FUNCTION();
		RZ_CORE_ASSERT(!s_Instance, "Application already exsits!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(RZ_BIND_EVENT_FN(Application::OnEvent));
		Renderer::Init();
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);	
	}

	void Application::OnEvent(Event& e)
	{
		RZ_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(RZ_BIND_EVENT_FN(Application::OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(RZ_BIND_EVENT_FN(Application::OnWindowResize));

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
		RZ_PROFILE_FUNCTION();
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
		RZ_PROFILE_FUNCTION();
		while (m_Running)
		{
			RZ_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime(); // Platform::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				RZ_PROFILE_SCOPE("LayerStack OnUpdate");
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(timestep);
				}
			}
			{
				RZ_PROFILE_SCOPE("LayerStack OnImGuiUpdate");
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

	void Application::PushLayer(Layer* layer)
	{
		RZ_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		RZ_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}


}

