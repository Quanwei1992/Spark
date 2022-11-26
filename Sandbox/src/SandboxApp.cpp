#include "Razor.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Razor::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
		,m_CameraController(1920.0f / 1080.0f,true)
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


		m_Shader = Razor::Shader::Create("TriangleShader", vertexSrc, fragmentSrc);

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
		m_FlatShader = Razor::Shader::Create("FlastShader", vertexSrc, fragmentSrc);

		m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Razor::Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Razor::Texture2D::Create("assets/textures/Logo.png");
	}

	void OnUpdate(Razor::Timestep ts) override
	{
		m_CameraController.OnUpdate(ts);
		Razor::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Razor::RenderCommand::Clear();

		Razor::Renderer::BeginScene(m_CameraController.GetCamera());

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
		m_LogoTexture->Bind(1);

		auto textureShader = m_ShaderLibrary.Get("Texture");

		textureShader->Bind();
		textureShader->UploadUniformInt("u_Texture", 0);
		Razor::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f),glm::vec3(1.5f)));

		
		textureShader->Bind();
		textureShader->UploadUniformInt("u_Texture", 1);
		Razor::Renderer::Submit(textureShader, m_SquareVA,
			glm::translate(glm::mat4(1.0f),glm::vec3(0.25f,-0.25f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		// Triangle
		//Razor::Renderer::Submit(m_Shader, m_VertexArray);

		Razor::Renderer::EndScene();
	}

	void OnEvent(Razor::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

	virtual void OnImGuiRender()
	{
		ImGui::Begin("Setttings");
		ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
private:
	Razor::ShaderLibrary m_ShaderLibrary;
	Razor::Ref<Razor::Shader> m_Shader;
	Razor::Ref<Razor::VertexArray> m_VertexArray;
	Razor::Ref<Razor::Shader> m_FlatShader;
	Razor::Ref<Razor::VertexArray> m_SquareVA;
	Razor::Ref<Razor::Texture2D> m_Texture,m_LogoTexture;
	Razor::OrthographicCameraController m_CameraController;
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


