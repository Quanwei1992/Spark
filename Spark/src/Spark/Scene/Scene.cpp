#include "skpch.h"
#include "Scene.h"
#include "Components.h"
#include "Spark/Renderer/Renderer2D.h"
#include <glm/glm.hpp>


namespace Spark
{

	static void DoMaths(const glm::mat4& transform)
	{

	}

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}
	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Registry.group<TransformCompoent>(entt::get<SpriteRendererCompoent>);
		for (auto entity : group )
		{
			auto& [transform, sprite] = group.get<TransformCompoent, SpriteRendererCompoent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}
	entt::entity Scene::CreateEntity()
	{
		return m_Registry.create();
	}
}

