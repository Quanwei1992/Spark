#include "Sandbox2D.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Random.h"


static const uint32_t s_MapWidth = 38;
static const char s_MapTiles[] =
"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWDDDDDDDDDDDDDDWWWWWWWWWWWWWWWW"
"WWWWWWWWDDDDDDDDDDDDDDWWWWWWWWWWWWWWWW"
"WWWWWWWWDDDDDDDDDDDDDDWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDDWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
;

SandBox2D::SandBox2D()
	:Layer("SandBox2D")
	,m_CameraController(true)
	,m_ParticleSystem(1000)
{
}

SandBox2D::~SandBox2D()
{
}

void SandBox2D::OnAttach()
{
	SK_PROFILE_FUNCTION();

	m_CheckerboradTexture = Spark::Texture2D::Create("assets/textures/Checkerboard.png");
	m_SpriteSheet = Spark::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

	m_TextureMap['W'] = Spark::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11,11 }, { 128,128 });
	m_TextureMap['D'] = Spark::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6,11 }, { 128,128 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = sizeof(s_MapTiles) / s_MapWidth;


	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.1f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

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
		Spark::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Spark::RenderCommand::Clear();
	}
	{
		SK_PROFILE_SCOPE("Renderer Draw");
		
#if 0
		{
			Spark::Renderer2D::BeginScene(m_CameraController.GetCamera());
			Spark::Renderer2D::DrawQuad({ -1.0f,0 }, { 0.8f,0.8f }, { 0.8f,0.2f,0.3f,1.0f });
			Spark::Renderer2D::DrawQuad({ 0.5f,0.5f }, { 0.5f,0.75f }, { 0.2f,0.8f,0.3f,1.0f });
			m_blueQuadRotation += ts * 180.0f;
			Spark::Renderer2D::DrawRotatedQuad({ 0.5f,-0.5f }, { 0.5f,1 }, glm::radians(m_blueQuadRotation), { 0.2f,0.3f,0.8f,1.0f });
			Spark::Renderer2D::DrawQuad({ 0,0,-0.1f }, { 10,10 }, m_CheckerboradTexture, 10.0f, { 1.0f,0.8f,0.8f,1.0f });
			Spark::Renderer2D::EndScene();
		}

		{
			Spark::Renderer2D::BeginScene(m_CameraController.GetCamera());
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
#endif // 0

		

	
		{
			Spark::Renderer2D::BeginScene(m_CameraController.GetCamera());
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
				for (int i = 0; i < 5; i++)
					m_ParticleSystem.Emit(m_Particle);
			}

			m_ParticleSystem.OnUpdate(ts);
			m_ParticleSystem.OnRender();
			Spark::Renderer2D::EndScene();
		}
		

		Spark::Renderer2D::BeginScene(m_CameraController.GetCamera());
		{
			for (uint32_t y = 0; y < m_MapHeight; y++)
			{
				for (uint32_t x = 0; x < m_MapWidth; x++)
				{
					auto subTexture = m_TextureMap[s_MapTiles[y * m_MapWidth + x]];
					glm::vec3 position = { 0,0,0 };
					position.x = (float)x - (float)m_MapWidth / 2.0f;
					position.y = (float)m_MapHeight / 2.0f - (float)y;
					Spark::Renderer2D::DrawQuad(position, { 1,1 }, subTexture, 1.0f, glm::vec4(1.0f));
				}
			}
		}
		Spark::Renderer2D::EndScene();
	}

}

void SandBox2D::OnImGuiRender()
{
	SK_PROFILE_FUNCTION();


	static bool open = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.

			if (ImGui::MenuItem("Exit"))
			{
				Spark::Application::Get().Exit();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}


	ImGui::Begin("Renderer2D Stats");
	auto stats = Spark::Renderer2D::GetStats();
	ImGui::Text("Draw Calls %d", stats.DrawCalls);
	ImGui::Text("Quads %d", stats.QuadCount);
	ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices %d", stats.GetTotalIndexCount());

	uint64_t rendererID = (uint64_t)m_CheckerboradTexture->GetRendererID();

	ImGui::Image((void*)rendererID, ImVec2{ 256.0f,256.0f });


	ImGui::End();


	ImGui::End();

}

void SandBox2D::OnEvent(Spark::Event& event)
{
	m_CameraController.OnEvent(event);
}
