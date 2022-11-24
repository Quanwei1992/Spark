#include "Razor.h"
#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

		Razor::Ref<Razor::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(Razor::VertexBuffer::Create(vertices, sizeof(vertices)));

		m_VertexBuffer->SetLayout({
			{Razor::ShaderDataType::Float3,"a_Position"},
			{Razor::ShaderDataType::Float4,"a_Color"}
			});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[3] = { 0,1,2 };

		Razor::Ref<Razor::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Razor::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		// Square 

		m_SquareVA.reset(Razor::VertexArray::Create());

		float squareVertices[5 * 4] = {
			-0.5f,-0.5f,0.0f,0,0,
			 0.5f,-0.5f,0.0f,1,0,
			 0.5f, 0.5f,0.0f,1,1,
			-0.5f, 0.5f,0.0f,0,1
		};
		Razor::Ref<Razor::VertexBuffer> squareVB;
		squareVB.reset(Razor::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
				{Razor::ShaderDataType::Float3,"a_Position"},
				{Razor::ShaderDataType::Float2,"a_TexCoord"},
			});

		m_SquareVA->AddVertexBuffer(squareVB);


		uint32_t squareIndices[] = { 0,1,2,2,3,0 };
		Razor::Ref<Razor::IndexBuffer> squareIB;
		squareIB.reset(Razor::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position,1.0);
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


		m_Shader.reset(Razor::Shader::Create(vertexSrc, fragmentSrc));

		// Flat Shader 
		// -------------------------------------------------------------
		vertexSrc = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position,1.0);
			}
		)";


		fragmentSrc = R"(
			#version 460 core

			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			uniform vec4 u_Color;

			void main()
			{
				color = u_Color;
			}
		)";
		m_FlatShader.reset(Razor::Shader::Create(vertexSrc, fragmentSrc));

		// Texture Shader 
		// -------------------------------------------------------------
		vertexSrc = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position,1.0);
			}
		)";


		fragmentSrc = R"(
			#version 460 core

			layout(location = 0) out vec4 color;
			in vec2 v_TexCoord;
			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture,v_TexCoord);
			}
		)";
		m_TextureShader.reset(Razor::Shader::Create(vertexSrc, fragmentSrc));

		m_Texture = Razor::Texture2D::Create("assets/textures/Checkerboard.png");
	}

	void OnUpdate(Razor::Timestep ts) override
	{
		RZ_TRACE("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());

		if (Razor::Input::IsKeyPressed(RZ_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_UP))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_A))
		{
			m_CameraRotation += m_CameraRotateSpeed * ts;
		}
		else if (Razor::Input::IsKeyPressed(RZ_KEY_D))
		{
			m_CameraRotation -= m_CameraRotateSpeed * ts;
		}

		Razor::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Razor::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Razor::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for (int x = 0; x < 15; x++) 
		{
			for (int y = 0; y < 15; y++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				m_FlatShader->UploadUniformFloat4("u_Color", m_SquareColor);
				Razor::Renderer::Submit(m_FlatShader, m_SquareVA, transform);
			}
		}
		m_Texture->Bind(0);
		m_TextureShader->Bind();
		m_TextureShader->UploadUniformInt("u_Texture", 0);
		Razor::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f),glm::vec3(1.5f)));
		// Triangle
		//Razor::Renderer::Submit(m_Shader, m_VertexArray);

		Razor::Renderer::EndScene();
	}

	void OnEvent(Razor::Event& event) override
	{
		Razor::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Razor::KeyPressedEvent>(RZ_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
	}


	bool OnKeyPressedEvent(Razor::KeyPressedEvent e)
	{
		return false;
	}

	virtual void OnImGuiRender()
	{
		ImGui::Begin("Setttings");
		ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
private:
	Razor::Ref<Razor::Shader> m_Shader;
	Razor::Ref<Razor::VertexArray> m_VertexArray;
	Razor::Ref<Razor::Shader> m_FlatShader,m_TextureShader;
	Razor::Ref<Razor::VertexArray> m_SquareVA;
	Razor::Ref<Razor::Texture2D> m_Texture;
	Razor::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition = { 0,0,0 };
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotateSpeed = 180.0f;
	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };
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


