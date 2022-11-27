#include "Sandbox2D.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SandBox2D::SandBox2D()
	:Layer("SandBox2D")
	, m_CameraController(1920.0f / 1080.0f, true)
{
}

SandBox2D::~SandBox2D()
{
}

void SandBox2D::OnAttach()
{
	m_SquareVA = Razor::VertexArray::Create();

	float squareVertices[5 * 3] = {
		-0.5f,-0.5f,0.0f,
		 0.5f,-0.5f,0.0f,
		 0.5f, 0.5f,0.0f,
		-0.5f, 0.5f,0.0f,
	};
	Razor::Ref<Razor::VertexBuffer> squareVB;
	squareVB.reset(Razor::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

	squareVB->SetLayout({
			{Razor::ShaderDataType::Float3,"a_Position"}
	});

	m_SquareVA->AddVertexBuffer(squareVB);
	uint32_t squareIndices[] = { 0,1,2,2,3,0 };
	Razor::Ref<Razor::IndexBuffer> squareIB;
	squareIB.reset(Razor::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_SquareVA->SetIndexBuffer(squareIB);


	m_FlatColorShader = Razor::Shader::Create("assets/shaders/FlatColorShader.glsl");
}

void SandBox2D::OnDetach()
{
	
}

void SandBox2D::OnUpdate(Razor::Timestep ts)
{
	m_CameraController.OnUpdate(ts);
	Razor::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
	Razor::RenderCommand::Clear();

	Razor::Renderer2D::BeginScene(m_CameraController.GetCamera());
	{
		Razor::Renderer2D::DrawQuad({ 0,0 }, { 1,1 }, {0.8f,0.2f,0.3f,1.0f});
	}
	Razor::Renderer::EndScene();

	//m_FlatColorShader->Bind();
//m_FlatColorShader->UploadUniformFloat4("u_Color", m_SquareColor);

//Razor::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("Setttings");
	ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void SandBox2D::OnEvent(Razor::Event& event)
{
	m_CameraController.OnEvent(event);
}
