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
	m_CheckerboradTexture = Razor::Texture2D::Create("assets/textures/Checkerboard.png");
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
		Razor::Renderer2D::DrawQuad({ -1.0f,0 }, { 0.8f,0.8f }, { 0.8f,0.2f,0.3f,1.0f });
		Razor::Renderer2D::DrawQuad({ 0.5f,-0.5f }, { 0.5f,1 }, {0.2f,0.3f,0.8f,1.0f});
		Razor::Renderer2D::DrawQuad({ 0,0,-0.1f }, { 10,10 }, m_CheckerboradTexture);
	}
	Razor::Renderer::EndScene();
}

void SandBox2D::OnImGuiRender()
{

}

void SandBox2D::OnEvent(Razor::Event& event)
{
	m_CameraController.OnEvent(event);
}