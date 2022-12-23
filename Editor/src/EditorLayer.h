#pragma once

#include "Panels/SceneHierarchyPanel.h"
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
		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		OrthographicCameraController m_CameraController;
		Ref<Framebuffer> m_Framebuffer;
		float m_blueQuadRotation = 0.0f;
		glm::vec2 m_ViewportSize = {1920,1080};
		bool m_ViewportFocused = false;
		bool m_ViewportHoverd = false;
		int m_GizmoType = 0;
		EditorCamera m_EditorCamera;
		Ref<Scene> m_ActiveScene;
		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}

