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
	SK_PROFILE_FUNCTION();

	m_CheckerboradTexture = Spark::Texture2D::Create("assets/textures/Checkerboard.png");
}

void SandBox2D::OnDetach()
{
	SK_PROFILE_FUNCTION();
}

void SandBox2D::OnUpdate(Spark::Timestep ts)
{
	SK_PROFILE_FUNCTION();

	m_CameraController.OnUpdate(ts);

	{
		SK_PROFILE_SCOPE("Renderer Prep");
		Spark::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Spark::RenderCommand::Clear();
	}
	{
		SK_PROFILE_SCOPE("Renderer Draw");
		Spark::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Spark::Renderer2D::DrawQuad({ -1.0f,0 }, { 0.8f,0.8f }, {0.8f,0.2f,0.3f,1.0f });
		m_blueQuadRotation += ts * 180.0f;
		Spark::Renderer2D::DrawRotatedQuad({ 0.5f,-0.5f }, { 0.5f,1 }, glm::radians(m_blueQuadRotation), {0.2f,0.3f,0.8f,1.0f});
		Spark::Renderer2D::DrawQuad({ 0,0,-0.1f }, { 10,10 }, m_CheckerboradTexture, 10.0f, {1.0f,0.8f,0.8f,1.0f});

		Spark::Renderer::EndScene();
	}

}

void SandBox2D::OnImGuiRender()
{
	SK_PROFILE_FUNCTION();
	ImGui::Begin("SandBox2D");
	ImGui::End();
}

void SandBox2D::OnEvent(Spark::Event& event)
{
	m_CameraController.OnEvent(event);
}
