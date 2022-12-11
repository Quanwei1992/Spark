#include "skpch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"

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
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create() ,this };
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		entity.AddComponent<TransformCompoent>();

		return entity;
	}
}

