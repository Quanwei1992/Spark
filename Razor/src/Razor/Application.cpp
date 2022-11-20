#include "rzpch.h"
#include "Application.h"
#include "Razor/Log.h"
#include "Razor/Events/Input.h"
#include "Razor/ImGui/ImGuiLayer.h"

#include "Razor/Renderer/Shader.h"
#include "Razor/Renderer/Buffer.h"

#include <glad/glad.h>


namespace Razor
{

	Application* Application::s_Instance = nullptr;

	#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)

	Application::Application()
	{
		RZ_CORE_ASSERT(!s_Instance, "Application already exsits!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);


		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);
	
		float vertices[3 * 3] = {
			-0.5f,-0.5f,0.0f,
			0.5f,-0.5f,0.0f,
			0.0f,0.5f,0.0f
		};


		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		uint32_t indices[3] = { 0,1,2 };

		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));



		std::string vertexSrc = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position,1.0);
			}
		)";


		std::string fragmentSrc = R"(
			#version 460 core

			layout(location = 0) out vec4 color;
			in vec3 v_Position;

			void main()
			{
				color = vec4(v_Position+0.5,1.0);
			}
		)";


		m_Shader.reset(new Shader(vertexSrc,fragmentSrc));
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClosed));

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


	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			m_Window->OnUpdate();
		};
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}


}

