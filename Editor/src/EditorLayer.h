#pragma once

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
		OrthographicCameraController m_CameraController;
		Ref<Texture2D> m_CheckerboradTexture;
		Ref<Framebuffer> m_Framebuffer;
		float m_blueQuadRotation = 0.0f;
		glm::vec2 m_ViewportSize = {0,0};
		bool m_ViewportFocused = false;
		bool m_ViewportHoverd = false;
	};
}

