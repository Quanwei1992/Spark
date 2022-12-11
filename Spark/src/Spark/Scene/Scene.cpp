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

		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4* mainCameraTransform = nullptr;
		auto view = m_Registry.view<TransformComponent,CameraComponent>();
		for (auto&& [entity, transfrom, camera] : view.each()) {
			if (camera.Primary) {
				mainCamera = &camera.Camera;
				mainCameraTransform = &transfrom.Transform;
				break;
			}
		}

		if (!mainCamera)
		{
			return;
		}

		Renderer2D::BeginScene(*mainCamera, *mainCameraTransform);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group )
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);
		}

		Renderer2D::EndScene();
	}
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create() ,this };
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		entity.AddComponent<TransformComponent>();

		return entity;
	}
}

