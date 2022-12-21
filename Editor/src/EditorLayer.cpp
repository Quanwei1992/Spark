#include "EditorLayer.h"
#include "Spark/Scene/SceneSerializer.h"
#include "Spark/Utils/PlatformUtils.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Spark
{
	EditorLayer::EditorLayer()
		:Layer("EditorLayer")
		, m_CameraController(16.0f/9.0f,true)
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		SK_PROFILE_FUNCTION();

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1920;
		fbSpec.Height = 1080;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_CameraController.SetZoomLevel(5.0f);
		m_CameraController.OnResize(fbSpec.Width, fbSpec.Height);
		m_ActiveScene = CreateRef<Scene>();

		//m_SquareEntity = m_ActiveScene->CreateEntity("Square");
		//m_SquareEntity.AddComponent<SpriteRendererComponent>(Color4f{ 0.0f,1.0f,0.0f,1.0f });

		//m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		//m_CameraEntity.AddComponent<CameraComponent>();

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate() override
			{
				
			}
			void OnDestory() override
			{
				
			}
			void OnUpdate(Timestep ts) override
			{
				auto& transform = GetComponent<TransformComponent>();
				float speed = 5.0f;
				if (Input::IsKeyPressed(SK_KEY_A))
				{
					transform.Translation.x -= speed * ts;
				}
				if (Input::IsKeyPressed(SK_KEY_D))
				{
					transform.Translation.x += speed * ts;
				}
				if (Input::IsKeyPressed(SK_KEY_W))
				{
					transform.Translation.y += speed * ts;
				}
				if (Input::IsKeyPressed(SK_KEY_S))
				{
					transform.Translation.y -= speed * ts;
				}
			}
		};

		//m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		SK_PROFILE_FUNCTION();
	}


	void EditorLayer::OnUpdate(Spark::Timestep ts)
	{
		SK_PROFILE_FUNCTION();	

		// Resize
		const FramebufferSpecification& fbSpec = m_Framebuffer->GetSpecification();
		if (fbSpec.Width != (uint32_t)m_ViewportSize.x || fbSpec.Height != (uint32_t)m_ViewportSize.y)
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Update
		if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts);


		// Render
		Spark::Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		Spark::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Spark::RenderCommand::Clear();
		// Update scene
		m_ActiveScene->OnUpdate(ts);

		m_Framebuffer->Unbind();

	}

	void EditorLayer::OnImGuiRender()
	{
		SK_PROFILE_FUNCTION();

		static bool dockingEnabled = true;
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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSize = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;


		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}


		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New","Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...","Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save As...","Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit"))
				{
					Spark::Application::Get().Exit();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Settings");
		{
			auto stats = Spark::Renderer2D::GetStats();
			ImGui::Text("Draw Calls %d", stats.DrawCalls);
			ImGui::Text("Quads %d", stats.QuadCount);
			ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices %d", stats.GetTotalIndexCount());
		}
		ImGui::End();




		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("Viewport");
		{
			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHoverd = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHoverd);
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if ((viewportPanelSize.x >0 && viewportPanelSize.y > 0) && 
				((uint32_t)viewportPanelSize.x != (uint32_t)m_ViewportSize.x || (uint32_t)viewportPanelSize.y != (uint32_t)m_ViewportSize.y))
			{
				m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			}

			uint64_t rendererID = (uint64_t)m_Framebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)rendererID, viewportPanelSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });

		}
		ImGui::End();
		ImGui::PopStyleVar();

		

		ImGui::End();
	}

	void EditorLayer::OnEvent(Spark::Event& e)
	{
		m_CameraController.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SK_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatcount() > 0) return 0;
		bool control = Input::IsKeyPressed(SK_KEY_LEFT_CONTROL) || Input::IsKeyPressed(SK_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(SK_KEY_LEFT_SHIFT) || Input::IsKeyPressed(SK_KEY_RIGHT_SHIFT);
		switch (e.GetKeyCode())
		{
		case SK_KEY_O:
		{
			if (control)
			{
				OpenScene();
			}
			break;
		}
		case SK_KEY_S:
		{
			if (control && shift)
			{
				SaveSceneAs();
			}
			break;
		}
		case SK_KEY_N:
		{
			if (control)
			{
				NewScene();
			}
			break;
		}
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		auto filepath = FileDialogs::OpenFile("Spark Scene (*.spark)\0*.spark\0");
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		auto filepath = FileDialogs::SaveFile("Spark Scene (*.spark)\0*.spark\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

}


