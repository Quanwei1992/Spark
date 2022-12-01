#include "rzpch.h"
#include "OrthographicCameraController.h"

#include "Spark/Events/Input.h"
#include "Spark/Core/KeyCodes.h"

namespace Spark
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio)
		, m_ZoomLevel(1.0f)
		, m_Rotation(rotation)
		, m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel,-m_ZoomLevel,m_ZoomLevel)
	{
	}
	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		SK_PROFILE_FUNCTION();

		if (Input::IsKeyPressed(SK_KEY_A))
		{
			m_CameraPosition.x -= m_CameraTranslationSpeed * m_ZoomLevel * ts;
		}
		else if (Input::IsKeyPressed(SK_KEY_D))
		{
			m_CameraPosition.x += m_CameraTranslationSpeed * m_ZoomLevel * ts;
		}
		else if (Input::IsKeyPressed(SK_KEY_S))
		{
			m_CameraPosition.y -= m_CameraTranslationSpeed * m_ZoomLevel * ts;
		}
		else if (Input::IsKeyPressed(SK_KEY_W))
		{
			m_CameraPosition.y += m_CameraTranslationSpeed * m_ZoomLevel * ts;
		}
		if (m_Rotation)
		{
			if (Input::IsKeyPressed(SK_KEY_Q))
			{
				m_CameraRotation += m_CameraRotationSpeed * ts;
			}
			else if (Input::IsKeyPressed(SK_KEY_E))
			{
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			}
			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

	}
	void OrthographicCameraController::OnEvent(Event& e)
	{
		

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(SK_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
		dispatcher.Dispatch<MouseScrolledEvent>(SK_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
	}
	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		SK_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.5f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		SK_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}