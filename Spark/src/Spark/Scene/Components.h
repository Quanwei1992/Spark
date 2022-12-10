#pragma once

#include <glm/glm.hpp>

namespace Spark
{
	struct TransformCompoent
	{
		glm::mat4 Transform = glm::mat4(1.0f);

		TransformCompoent() = default;
		TransformCompoent(const TransformCompoent&) = default;
		TransformCompoent(const glm::mat4& transform)
			: Transform(transform) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	struct SpriteRendererCompoent
	{
		Color4f Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererCompoent() = default;
		SpriteRendererCompoent(const SpriteRendererCompoent&) = default;
		SpriteRendererCompoent(const Color4f& color)
			: Color(color) {}
	};
	
}