#pragma once
#include "Spark/Renderer/OrthographicCamera.h"
#include "Spark/Core/Timestep.h"
#include "Spark/Events/ApplicationEvent.h"
#include "Spark/Events/MouseEvent.h"

#include<glm/glm.hpp>

namespace Spark
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio,bool rotation = false);
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
	public:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel;
		bool m_Rotation;
		OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition = {0,0,0};
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 2.5f, m_CameraRotationSpeed = 90.0f;
	};
}