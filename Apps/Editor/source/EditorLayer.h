#pragma once

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "Spark/Events/KeyEvent.h"
#include "Spark/Renderer/EditorCamera.h"

#include <Spark.h>

namespace Spark
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event)override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MouseButtonPressedEvent& e);
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		void OnOverlayRender();

		Ref<Scene> GetActiveScene();

	private:
		void UI_Toolbar();
		void DrawGizoms();
	private:
		OrthographicCameraController m_CameraController;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Texture2D> m_IconPlay, m_IconStop;
		float m_blueQuadRotation = 0.0f;
		glm::vec2 m_ViewportSize = {1920,1080};
		glm::vec2 m_ViewportBounds[2];
		bool m_ViewportFocused = false;
		bool m_ViewportHoverd = false;
		int m_GizmoType = 0;
		bool m_ShowPhysicsColliders = false;
		Entity m_HoveredEntity = Entity::Empty;
		EditorCamera m_EditorCamera;
		std::filesystem::path m_EditorScenePath;
		Ref<Scene> m_EditorScene,m_RuntimeScene;
		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		Entity m_MeshEntity;
		Ref<Material> m_MeshMaterial;


		enum class SceneState
		{
			Edit = 0,Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		// TEMP PBR PARAMS

		struct AlbedoInput
		{
			glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; // Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
			Ref<Texture2D> TextureMap;
			bool SRGB = true;
			bool UseTexture = false;
		};
		AlbedoInput m_AlbedoInput;

		struct NormalInput
		{
			Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		NormalInput m_NormalInput;

		struct MetalnessInput
		{
			float Value = 1.0f;
			Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		MetalnessInput m_MetalnessInput;

		struct RoughnessInput
		{
			float Value = 0.2f;
			Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		RoughnessInput m_RoughnessInput;

		struct Light
		{
			glm::vec3 Direction;
			glm::vec3 Radiance;
		};
		Light m_Light;
		float m_LightMultiplier = 0.3f;

		// PBR params
		bool m_RadiancePrefilter = false;

		float m_EnvMapRotation = 0.0f;
	};
}

