#pragma once

#include "Spark/Core/Timestep.h"

#include <glm.hpp>

namespace Spark
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projectionMatrix);

		void Focus();
		void Update(Timestep ts);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
		inline void SetViewportSize(uint32_t width, uint32_t height) { m_ViewportWidth = width; m_ViewportHeight = height; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();
		const glm::vec3& GetPosition() const { return m_Position; }

		float GetExposure() const { return m_Exposure; }
		void SetExposure(float value) { m_Exposure = value; }

	private:
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);
		glm::vec3 CalculatePosition();
		glm::quat GetOrientation();
		glm::vec2 PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	protected:
		glm::mat4 m_ProjectionMatrix, m_ViewMatrix;
		glm::vec3 m_Position, m_Rotation, m_FocalPoint;

		bool m_Panning, m_Rotating;
		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;
		float m_Exposure = 0.8f;
		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1920, m_ViewportHeight = 1080;
	};
}
