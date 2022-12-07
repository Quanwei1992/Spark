#include "Sandbox2D.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Random.h"



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


	Spark::FramebufferSpecification fbSpec;
	fbSpec.Width = 1204;
	fbSpec.Height = 576;

	m_Framebuffer = Spark::Framebuffer::Create(fbSpec);
	m_CheckerboradTexture = Spark::Texture2D::Create("assets/textures/Checkerboard.png");

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
		m_Framebuffer->Bind();
		Spark::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Spark::RenderCommand::Clear();
	}
	{
		SK_PROFILE_SCOPE("Renderer Draw");
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

		m_Framebuffer->Unbind();
	}

}

void SandBox2D::OnImGuiRender()
{
	SK_PROFILE_FUNCTION();


	static bool dockingEnabled = true;

	if (dockingEnabled)
	{
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
		ImGui::Begin("DockSpace Demo", &dockingEnabled, window_flags);
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
		uint64_t rendererID = (uint64_t)m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)rendererID, ImVec2{ 1204,576 });
		ImGui::End();
		ImGui::End();
	}

}

void SandBox2D::OnEvent(Spark::Event& event)
{
	m_CameraController.OnEvent(event);
}
