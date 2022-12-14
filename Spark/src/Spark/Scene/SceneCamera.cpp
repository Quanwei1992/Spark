#include "skpch.h"

#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}
	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}


	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	void SceneCamera::RecalculateProjection()
	{

		if (m_ProjectionType == ProjectionType::Perspective)
		{

			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);

		}
		else if (m_ProjectionType == ProjectionType::Otrhographic)
		{
			float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoLeft = -orthoRight;
			float orthoTop = m_OrthographicSize * 0.5f;
			float orthoBottom = -orthoTop;

			m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		}


	}
}