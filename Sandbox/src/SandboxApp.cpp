#include "Sandbox2D.h"

#include <Spark.h>
#include <Spark/Core/EntryPoint.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Spark::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
		,m_CameraController(1920.0f / 1080.0f,true)
	{
		m_VertexArray = Spark::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f,-0.5f,0.0f,1.0f,0.0f,0.0f,1.0f,
			0.5f,-0.5f,0.0f,0.0f,1.0f,0.0f,1.0f,
			0.0f,0.5f,0.0f,0.0f,0.0f,1.0f,1.0f,
		};

		Spark::Ref<Spark::VertexBuffer> m_VertexBuffer = Spark::VertexBuffer::Create(vertices, sizeof(vertices));

		m_VertexBuffer->SetLayout({
			{Spark::ShaderDataType::Float3,"a_Position"},
			{Spark::ShaderDataType::Float4,"a_Color"}
			});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[3] = { 0,1,2 };

		Spark::Ref<Spark::IndexBuffer> m_IndexBuffer = Spark::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		// Square 

		m_SquareVA = Spark::VertexArray::Create();

		float squareVertices[5 * 4] = {
			-0.5f,-0.5f,0.0f,0,0,
			 0.5f,-0.5f,0.0f,1,0,
			 0.5f, 0.5f,0.0f,1,1,
			-0.5f, 0.5f,0.0f,0,1
		};
		Spark::Ref<Spark::VertexBuffer> squareVB = Spark::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

		squareVB->SetLayout({
				{Spark::ShaderDataType::Float3,"a_Position"},
				{Spark::ShaderDataType::Float2,"a_TexCoord"},
			});

		m_SquareVA->AddVertexBuffer(squareVB);


		uint32_t squareIndices[] = { 0,1,2,2,3,0 };
		Spark::Ref<Spark::IndexBuffer> squareIB = Spark::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
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


		m_Shader = Spark::Shader::Create("TriangleShader", vertexSrc, fragmentSrc);

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
		m_FlatShader = Spark::Shader::Create("FlastShader", vertexSrc, fragmentSrc);

		m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Spark::Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Spark::Texture2D::Create("assets/textures/Logo.png");
	}

	void OnUpdate(Spark::Timestep ts) override
	{
		m_CameraController.OnUpdate(ts);
		Spark::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Spark::RenderCommand::Clear();

		Spark::Renderer::BeginScene(m_CameraController.GetCamera());

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for (int x = 0; x < 15; x++) 
		{
			for (int y = 0; y < 15; y++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				m_FlatShader->SetFloat4("u_Color", m_SquareColor);
				Spark::Renderer::Submit(m_FlatShader, m_SquareVA, transform);
			}
		}
		m_Texture->Bind(0);
		m_LogoTexture->Bind(1);

		auto textureShader = m_ShaderLibrary.Get("Texture");

		textureShader->Bind();
		textureShader->SetInt("u_Texture", 0);
		Spark::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f),glm::vec3(1.5f)));

		
		textureShader->Bind();
		textureShader->SetInt("u_Texture", 1);
		Spark::Renderer::Submit(textureShader, m_SquareVA,
			glm::translate(glm::mat4(1.0f),glm::vec3(0.25f,-0.25f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		// Triangle
		//Spark::Renderer::Submit(m_Shader, m_VertexArray);

		Spark::Renderer::EndScene();
	}

	void OnEvent(Spark::Event& e) override
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
	Spark::ShaderLibrary m_ShaderLibrary;
	Spark::Ref<Spark::Shader> m_Shader;
	Spark::Ref<Spark::VertexArray> m_VertexArray;
	Spark::Ref<Spark::Shader> m_FlatShader;
	Spark::Ref<Spark::VertexArray> m_SquareVA;
	Spark::Ref<Spark::Texture2D> m_Texture,m_LogoTexture;
	Spark::OrthographicCameraController m_CameraController;
	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };
};


class Sandbox : public Spark::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new SandBox2D());
	}
};


Spark::Application* Spark::CreateApplication()
{
	return new Sandbox();
}


