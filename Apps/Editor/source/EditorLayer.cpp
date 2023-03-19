#include "EditorLayer.h"
#include "Spark/Scene/SceneSerializer.h"
#include "Spark/Utils/PlatformUtils.h"
#include "Spark/Math/Math.h"
#include "Spark/Core/MouseCodes.h"
#include "Spark/Profile/Instrumentor.h"

#include <imgui.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <ImGuizmo.h>

#include "Spark/Renderer/SceneRenderer.h"

namespace Spark
{
	EditorLayer::EditorLayer()
		:Layer("EditorLayer")
		, m_CameraController(16.0f/9.0f,true)
		,m_GizmoType(ImGuizmo::OPERATION::TRANSLATE)
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
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::RED_INTEGER,FramebufferTextureFormat::Depth };
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_CameraController.SetZoomLevel(5.0f);
		m_CameraController.OnResize(fbSpec.Width, fbSpec.Height);
		m_EditorScene = CreateRef<Scene>();

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			OpenScene(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 10000.0f);

		m_SceneHierarchyPanel.SetContext(m_EditorScene);

		m_IconPlay = Texture2D::Create("resources/icons/PlayButton.png");
		m_IconStop = Texture2D::Create("resources/icons/StopButton.png");

		auto environment = Environment::Load("assets/env/birchwood_4k.hdr");
		m_EditorScene->SetEnvironment(environment);


		m_MeshEntity = m_EditorScene->CreateEntity("Test Entity");

		auto mesh = CreateRef<Mesh>("assets/meshes/TestScene.fbx");
		m_MeshMaterial = mesh->GetMaterial();
		auto& meshComponent = m_MeshEntity.AddComponent<MeshRendererComponent>();
		meshComponent.Mesh = mesh;

		auto gunEntity = m_EditorScene->CreateEntity("Gun Entity");
		auto& gunTransform = gunEntity.GetComponent<TransformComponent>();
		gunTransform.Translation = {5,5,5};
		gunTransform.Scale = { 10,10,10 };

		// Set lights
		m_Light.Direction = { -0.5f, -0.5f, 1.0f };
		m_Light.Radiance = { 1.0f, 1.0f, 1.0f };
	}

	void EditorLayer::OnDetach()
	{
		SK_PROFILE_FUNCTION();
	}


	void EditorLayer::OnUpdate(Timestep ts)
	{
		SK_PROFILE_FUNCTION();	

		m_MeshMaterial->Set("u_AlbedoColor", m_AlbedoInput.Color);
		m_MeshMaterial->Set("u_Metalness", m_MetalnessInput.Value);
		m_MeshMaterial->Set("u_Roughness", m_RoughnessInput.Value);
		m_MeshMaterial->Set("lights", m_Light);
		m_MeshMaterial->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_EnvMapRotation", m_EnvMapRotation);


		Ref<Scene> activeScene = m_SceneState == SceneState::Edit ? m_EditorScene : m_RuntimeScene;


		// Resize
		const FramebufferSpecification& fbSpec = m_Framebuffer->GetSpecification();
		if (fbSpec.Width != (uint32_t)m_ViewportSize.x || fbSpec.Height != (uint32_t)m_ViewportSize.y)
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			activeScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
		
		// Render
		//Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		RenderCommand::Clear();

		// Clear our entity ID attachment to -1

		m_Framebuffer->ClearAttachment(1, -1);

		// Update scene

		switch (m_SceneState)
		{
		case SceneState::Edit:
		{
			// Update
			if (m_ViewportFocused)
			{
				m_CameraController.OnUpdate(ts);
			}
			m_EditorCamera.OnUpdate(ts);
			m_EditorScene->OnUpdateEditor(ts, m_EditorCamera);
		}break;
		case SceneState::Play:
			m_RuntimeScene->OnUpdateRuntime(ts);
			break;
		}

		OnOverlayRender();


		auto[mx,my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			if (pixelData == -1)
			{
				m_HoveredEntity = Entity::Empty;
			}
			else {
				m_HoveredEntity = {(entt::entity)pixelData,activeScene.get() };
			}
		}
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

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

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

				bool inEditMode = m_SceneState == SceneState::Edit;


				if (ImGui::MenuItem("Save", "Ctrl+S",false, inEditMode && !m_EditorScenePath.empty()))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Save As...","Ctrl+Shift+S",false,inEditMode))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit"))
				{
					Application::Get().Exit();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		//ImGui::Begin("Stats");
		//{
		//	auto stats = Renderer2D::GetStats();
		//	ImGui::Text("Draw Calls %d", stats.DrawCalls);
		//	ImGui::Text("Quads %d", stats.QuadCount);
		//	ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
		//	ImGui::Text("Indices %d", stats.GetTotalIndexCount());

		//	std::string name = "None";
		//	if (m_HoveredEntity)
		//	{
		//		name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		//	}
		//	ImGui::Text("Hovered Entity: %s", name.c_str());
		//}
		//ImGui::End();


		ImGui::Begin("Settings");
		{
			ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsColliders);
		}
		ImGui::End();


		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("Viewport");
		{
			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHoverd = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHoverd);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if ((viewportPanelSize.x >0 && viewportPanelSize.y > 0) && 
				((uint32_t)viewportPanelSize.x != (uint32_t)m_ViewportSize.x || (uint32_t)viewportPanelSize.y != (uint32_t)m_ViewportSize.y))
			{
				m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			}

			uint64_t rendererID = (uint64_t)SceneRenderer::GetFinalColorBufferRendererID();
			ImGui::Image((void*)rendererID, viewportPanelSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
				if (payload)
				{
					const wchar_t* path = (wchar_t*)payload->Data;
					OpenScene(path);
				}
				ImGui::EndDragDropTarget();
			}

			auto windowSize = ImGui::GetWindowSize();
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			DrawGizoms();

		}
		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::DrawGizoms()
	{
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (!selectedEntity || m_GizmoType == 0 || m_SceneState == SceneState::Play) return;
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);


		const auto& cameraProjection = m_EditorCamera.GetProjectionMatrix();
		glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

		// Entity Transform
		auto& tc = selectedEntity.GetComponent<TransformComponent>();
		glm::mat4 transform = tc.GetTransform();

		// Snapping
		bool snap = Input::IsKeyPressed(Key::LeftControl);
		float snapValue = 0.5f;
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
		{
			snapValue = 45.0f;
		}
		float snapValues[3] = { snapValue,snapValue ,snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);
			tc.Translation = translation;

			glm::vec3 deltaRotation = rotation - tc.Rotation;
			tc.Rotation += deltaRotation;
			tc.Scale = scale;
		}
	}



	void EditorLayer::UI_Toolbar()
	{
		auto& colors = ImGui::GetStyle().Colors;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_ButtonHovered]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_ButtonActive]);

		ImGui::Begin("##toolbar",nullptr,ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse| ImGuiWindowFlags_NoTitleBar);

		float size = ImGui::GetWindowHeight() - 4.0f;

		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
		ImVec4 tintColor = m_SceneState == SceneState::Edit ? ImVec4(0.54f, 0.88f, 0.54f, 1.0f) : ImVec4(0.75f, 0.25f, 0.28f, 1.0f);

		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		if (ImGui::ImageButton(icon->GetImGuiTextureID(), ImVec2(size, size),ImVec2(0,0),ImVec2(1,1),0,ImVec4(0,0,0,0), tintColor))
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
			else if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
			}
		}

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);

		ImGui::End();
	}


	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SK_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SK_BIND_EVENT_FN(EditorLayer::OnMousePressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatcount() > 0) return 0;
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
		case Key::O:
		{
			if (control)
			{
				OpenScene();
			}
			break;
		}
		case Key::S:
		{
			if (control && shift)
			{
				SaveSceneAs();
			}
			else if (control)
			{
				SaveScene();
			}
			break;
		}
		case Key::N:
		{
			if (control)
			{
				NewScene();
			}
			break;
		}

		case Key::D:
		{
			if (control) OnDuplicateEntity();
			break;
		}


		// Gizmos
		case Key::Q:
			m_GizmoType = 0;
			break;
		case Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		case Key::R:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		return false;
	}

	bool EditorLayer::OnMousePressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHoverd && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}		
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		if (m_SceneState != SceneState::Edit) OnSceneStop();

		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);
	}

	void EditorLayer::OpenScene()
	{
		auto filepath = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			OpenScene(filepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit) OnSceneStop();

		if (path.extension().string() != ".scene")
		{
			SK_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);

		SceneSerializer serializer(m_EditorScene);
		serializer.Deserialize(path.string());

		m_EditorScenePath = path;
	}

	void EditorLayer::SaveSceneAs()
	{
		if (m_SceneState != SceneState::Edit) return;
		auto filepath = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(filepath);
		}
	}

	void EditorLayer::SaveScene()
	{
		if (m_SceneState != SceneState::Edit) return;
		if (m_EditorScenePath.empty()) return;
		SceneSerializer serializer(m_EditorScene);
		serializer.Serialize(m_EditorScenePath.string());
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_RuntimeScene = Scene::Copy(m_EditorScene);
		m_RuntimeScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_RuntimeScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		m_RuntimeScene->OnRuntimeStop();
		m_RuntimeScene = nullptr;

		m_SceneHierarchyPanel.SetContext(m_EditorScene);

	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit) return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (!selectedEntity) return;

		m_EditorScene->DuplicateEntity(selectedEntity);
	}

	void EditorLayer::OnOverlayRender()
	{
		//auto activeScene = GetActiveScene();

		//if (m_SceneState == SceneState::Play)
		//{
		//	auto cameraEntity = activeScene->GetPrimaryCameraEntity();
		//	if (!cameraEntity) return;

		//	auto transform = cameraEntity.GetComponent<TransformComponent>();
		//	auto mainCamera = cameraEntity.GetComponent<CameraComponent>().Camera;

		//	glm::mat4 viewProjection = mainCamera.GetProjectionMatrix() * glm::inverse(transform.GetTransform());

		//	Renderer2D::BeginScene(viewProjection);
		//}
		//else {
		//	Renderer2D::BeginScene(m_EditorCamera.GetViewProjection());
		//}

		//if (m_ShowPhysicsColliders)
		//{
		//	// Circle Colliders
		//	{
		//		auto view = activeScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
		//		for (auto&& [entity, tc, cc2d] : view.each())
		//		{
		//			glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
		//			glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);


		//			glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
		//				* glm::scale(glm::mat4(1.0f), scale);

		//			Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
		//		}
		//	}
		//	// Box Colliders
		//	{
		//		auto view = activeScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
		//		for (auto&& [entity, tc, bc2d] : view.each())
		//		{
		//			glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
		//			glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);


		//			glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
		//				* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0, 0, 1))
		//				* glm::scale(glm::mat4(1.0f), scale);

		//			Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
		//		}
		//	}
		//}

	
		//Renderer2D::EndScene();
	}

	Ref<Scene> EditorLayer::GetActiveScene()
	{
		return m_SceneState == SceneState::Edit ? m_EditorScene : m_RuntimeScene;
	}

}


