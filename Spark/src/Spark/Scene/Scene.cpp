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
		// Update scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto&& [entity,nsc] : view.each()) {
				if (nsc.Instance == nullptr)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity ,this};
					nsc.Instance->OnCreate();
				}
				nsc.Instance->OnUpdate(ts);
			}
		}


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

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto&& [entity,camera] : view.each()) {
			if (!camera.FixedAspectRatio)
			{
				camera.Camera.SetViewportSize(width, height);
			}
		}
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

