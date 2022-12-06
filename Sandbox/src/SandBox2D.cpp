#include "Sandbox2D.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Random.h"

SandBox2D::SandBox2D()
	:Layer("SandBox2D")
	,m_CameraController(true)
	,m_ParticleSystem(99999)
{
}

SandBox2D::~SandBox2D()
{
}

void SandBox2D::OnAttach()
{
	SK_PROFILE_FUNCTION();

	m_CheckerboradTexture = Spark::Texture2D::Create("assets/textures/Checkerboard.png");

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.1f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
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
		Spark::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Spark::RenderCommand::Clear();
	}
	{
		SK_PROFILE_SCOPE("Renderer Draw");
		Spark::Renderer2D::BeginScene(m_CameraController.GetCamera());
		{
			Spark::Renderer2D::DrawQuad({ -1.0f,0 }, { 0.8f,0.8f }, { 0.8f,0.2f,0.3f,1.0f });
			Spark::Renderer2D::DrawQuad({ 0.5f,0.5f }, { 0.5f,0.75f }, { 0.2f,0.8f,0.3f,1.0f });
			m_blueQuadRotation += ts * 180.0f;
			Spark::Renderer2D::DrawRotatedQuad({ 0.5f,-0.5f }, { 0.5f,1 }, glm::radians(m_blueQuadRotation), { 0.2f,0.3f,0.8f,1.0f });
			Spark::Renderer2D::DrawQuad({ 0,0,-0.1f }, { 10,10 }, m_CheckerboradTexture, 10.0f, { 1.0f,0.8f,0.8f,1.0f });
		}
		Spark::Renderer2D::EndScene();

		Spark::Renderer2D::BeginScene(m_CameraController.GetCamera());
		{
			float quadSize = 0.5f;
			for (float y = -5.0f; y < 5.0f; y += quadSize)
			{
				for (float x = -5.0f; x < 5.0f; x += quadSize)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f,0.4f,(y + 5.0f) / 10.0f,1.0f };
					Spark::Renderer2D::DrawQuad({ x,y }, { quadSize * 0.9f,quadSize * 0.9f }, color);
				}
			}
		}
		Spark::Renderer2D::EndScene();


		Spark::Renderer2D::BeginScene(m_CameraController.GetCamera());
		{
			if (Spark::Input::IsMouseButtonPressed(SK_MOUSE_BUTTON_LEFT))
			{
				auto [x, y] = Spark::Input::GetMousePosition();
				auto width = Spark::Application::Get().GetWindow().GetWidth();
				auto height = Spark::Application::Get().GetWindow().GetHeight();

				auto bounds = m_CameraController.GetBounds();
				auto pos = m_CameraController.GetCamera().GetPosition();
				x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
				y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
				m_Particle.Position = { x + pos.x, y + pos.y };
				m_Particle.ColorBegin = GenerateRandomColor();
				m_Particle.ColorEnd = GenerateRandomColor();
				for (int i = 0; i < 50; i++)
					m_ParticleSystem.Emit(m_Particle);
			}

			m_ParticleSystem.OnUpdate(ts);
			m_ParticleSystem.OnRender();
		}

		Spark::Renderer2D::EndScene();

		
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
