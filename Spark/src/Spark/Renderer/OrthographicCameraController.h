#pragma once
#include "Spark/Renderer/OrthographicCamera.h"
#include "Spark/Core/Timestep.h"
#include "Spark/Events/ApplicationEvent.h"
#include "Spark/Events/MouseEvent.h"

#include<glm/glm.hpp>

namespace Spark
{

	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};


	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(bool rotation = false);
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }

		void SetZoomLevel(float level);
		float GetZoomLevel() const;

	private:
		void CalculateView();
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		OrthographicCameraBounds m_Bounds;
		float m_AspectRatio = 1.0f;
		float m_ZoomLevel = 1.0f;
		bool m_Rotation;
		OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition = {0,0,0};
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 2.5f, m_CameraRotationSpeed = 90.0f;
	};
}