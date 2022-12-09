#include "skpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

namespace Spark
{

	static void DoMaths(const glm::mat4& transform)
	{

	}


	struct TransformCompoent
	{
		glm::mat4 Transform;

		TransformCompoent() = default;
		TransformCompoent(const TransformCompoent&) = default;
		TransformCompoent(const glm::mat4& transform)
			: Transform(transform) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	Scene::Scene()
	{

		entt::entity entity = m_Registry.create();
		auto& transform = m_Registry.emplace<TransformCompoent>(entity,glm::mat4(1.0f));
		DoMaths(transform);
		
		
		//transform = m_Registry.get<TransformCompoent>(entity);

		//m_Registry.clear();
		auto view = m_Registry.view<TransformCompoent>();
		for (auto entity : view)
		{

		}

	}

	Scene::~Scene()
	{

	}
}

