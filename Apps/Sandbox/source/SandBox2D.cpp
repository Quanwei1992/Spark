#include "Sandbox2D.h"
#include "Random.h"

#include <Spark/Profile/Instrumentor.h>

#include <imgui.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>




SandBox2D::SandBox2D()
	:Layer("SandBox2D")
	,m_CameraController(true)
{
}

SandBox2D::~SandBox2D()
{
}

void SandBox2D::OnAttach()
{
	SK_PROFILE_FUNCTION();

	m_CheckerboradTexture = Spark::Texture2D::Create("assets/textures/Checkerboard.png");
	m_CameraController.SetZoomLevel(5.0f);
}

void SandBox2D::OnDetach()
{
	SK_PROFILE_FUNCTION();
}

inline glm::vec4 GenerateRandomColor()
{
	return { Random::Float(),Random::Float(),Random::Float(),1.0f };
}

void SandBox2D::OnUpdate(Spark::Timestep ts)
{
	SK_PROFILE_FUNCTION();

	// Update

	m_CameraController.OnUpdate(ts);

	// Render
	Spark::Renderer2D::ResetStats();

	{
		SK_PROFILE_SCOPE("Renderer Prep");
		//Spark::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Spark::Renderer::Clear(0.1f, 0.1f, 0.1f, 1);
	}
	{
		SK_PROFILE_SCOPE("Renderer Draw");
		{
			Spark::Renderer2D::BeginScene(m_CameraController.GetCamera().GetViewProjectionMatrix());
			Spark::Renderer2D::DrawQuad({ -1.0f,0 }, { 0.8f,0.8f }, { 0.8f,0.2f,0.3f,1.0f });
			Spark::Renderer2D::DrawQuad({ 0.5f,0.5f }, { 0.5f,0.75f }, { 0.2f,0.8f,0.3f,1.0f });
			m_blueQuadRotation += ts * 180.0f;
			Spark::Renderer2D::DrawRotatedQuad({ 0.5f,-0.5f }, { 0.5f,1 }, glm::radians(m_blueQuadRotation), { 0.2f,0.3f,0.8f,1.0f });
			Spark::Renderer2D::DrawQuad({ 0,0,-0.1f }, { 10,10 }, m_CheckerboradTexture, 10.0f, { 1.0f,0.8f,0.8f,1.0f });
			Spark::Renderer2D::EndScene();
		}

		{
			Spark::Renderer2D::BeginScene(m_CameraController.GetCamera().GetViewProjectionMatrix());
			float quadSize = 0.5f;
			for (float y = -5.0f; y < 5.0f; y += quadSize)
			{
				for (float x = -5.0f; x < 5.0f; x += quadSize)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f,0.4f,(y + 5.0f) / 10.0f,1.0f };
					Spark::Renderer2D::DrawQuad({ x,y }, { quadSize * 0.9f,quadSize * 0.9f }, color);
				}
			}
			Spark::Renderer2D::EndScene();
		}
	}

}

void SandBox2D::OnImGuiRender()
{
	SK_PROFILE_FUNCTION();

	ImGui::Begin("Renderer2D Stats");
	auto stats = Spark::Renderer2D::GetStats();
	ImGui::Text("Draw Calls %d", stats.DrawCalls);
	ImGui::Text("Quads %d", stats.QuadCount);
	ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices %d", stats.GetTotalIndexCount());
	ImGui::End();

}

void SandBox2D::OnEvent(Spark::Event& event)
{
	m_CameraController.OnEvent(event);
}
