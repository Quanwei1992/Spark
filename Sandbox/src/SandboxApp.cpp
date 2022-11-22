#include "Razor.h"
#include <imgui.h>

class ExampleLayer : public Razor::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
		,m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		m_VertexArray.reset(Razor::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f,-0.5f,0.0f,1.0f,0.0f,0.0f,1.0f,
			0.5f,-0.5f,0.0f,0.0f,1.0f,0.0f,1.0f,
			0.0f,0.5f,0.0f,0.0f,0.0f,1.0f,1.0f,
		};

		std::shared_ptr<Razor::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(Razor::VertexBuffer::Create(vertices, sizeof(vertices)));

		m_VertexBuffer->SetLayout({
			{Razor::ShaderDataType::Float3,"a_Position"},
			{Razor::ShaderDataType::Float4,"a_Color"}
			});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[3] = { 0,1,2 };

		std::shared_ptr<Razor::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Razor::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		// Square 

		m_SquareVA.reset(Razor::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f,-0.75f,0.0f,
			0.75f,-0.75f,0.0f,
			0.75f,0.75f,0.0f,
			-0.75f,0.75f,0.0f
		};
		std::shared_ptr<Razor::VertexBuffer> squareVB;
		squareVB.reset(Razor::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
				{Razor::ShaderDataType::Float3,"a_Position"},
			});

		m_SquareVA->AddVertexBuffer(squareVB);


		uint32_t squareIndices[] = { 0,1,2,2,3,0 };
		std::shared_ptr<Razor::IndexBuffer> squareIB;
		squareIB.reset(Razor::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position,1.0);
			}
		)";


		std::string fragmentSrc = R"(
			#version 460 core

			layout(location = 0) out vec4 color;
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";


		m_Shader.reset(new Razor::Shader(vertexSrc, fragmentSrc));

		vertexSrc = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position,1.0);
			}
		)";


		fragmentSrc = R"(
			#version 460 core

			layout(location = 0) out vec4 color;
			in vec3 v_Position;

			void main()
			{
				color = vec4(0.1,0.1,0.8,1.0);
			}
		)";


		m_BlueShader.reset(new Razor::Shader(vertexSrc, fragmentSrc));
	}

	void OnUpdate() override
	{

		if (Razor::Input::IsKeyPressed(RZ_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_UP))
		{
			m_CameraPosition.y += m_CameraMoveSpeed;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_A))
		{
			m_CameraRotation += m_CameraRotateSpeed;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_D))
		{
			m_CameraRotation -= m_CameraRotateSpeed;
		}


		Razor::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Razor::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Razor::Renderer::BeginScene(m_Camera);
		Razor::Renderer::Submit(m_BlueShader, m_SquareVA);
		Razor::Renderer::Submit(m_Shader, m_VertexArray);

		Razor::Renderer::EndScene();
	}

	void OnEvent(Razor::Event& event) override
	{
		Razor::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Razor::KeyPressedEvent>(RZ_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));

	}

	bool OnKeyPressedEvent(Razor::KeyPressedEvent e)
	{
		//if (e.GetKeyCode() == RZ_KEY_LEFT)
		//{
		//	m_CameraPosition.x -= m_CameraSpeed;
		//}
		//if (e.GetKeyCode() == RZ_KEY_RIGHT)
		//{
		//	m_CameraPosition.x += m_CameraSpeed;
		//}
		//if (e.GetKeyCode() == RZ_KEY_DOWN)
		//{
		//	m_CameraPosition.y -= m_CameraSpeed;
		//}
		//if (e.GetKeyCode() == RZ_KEY_UP)
		//{
		//	m_CameraPosition.y += m_CameraSpeed;
		//}
		return false;
	}

	virtual void OnImGuiRender()
	{

	}
private:
	std::shared_ptr<Razor::Shader> m_Shader;
	std::shared_ptr<Razor::VertexArray> m_VertexArray;
	std::shared_ptr<Razor::Shader> m_BlueShader;
	std::shared_ptr<Razor::VertexArray> m_SquareVA;
	Razor::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition = { 0,0,0 };
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 0.01f;
	float m_CameraRotateSpeed = 2.0f;

};


class Sandbox : public Razor::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
};


Razor::Application* Razor::CreateApplication()
{
	return new Sandbox();
}


