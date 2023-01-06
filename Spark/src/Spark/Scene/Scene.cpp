#include "skpch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"

#include "Spark/Renderer/Renderer2D.h"
#include <glm/glm.hpp>
#include <box2d/box2d.h>

namespace Spark
{

	namespace Utils
	{
		static b2BodyType ToBox2DBodyType(Rigidbody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
			case Spark::Rigidbody2DComponent::BodyType::Static:
				return b2_staticBody;
			case Spark::Rigidbody2DComponent::BodyType::Dynamic:
				return b2_dynamicBody;
			case Spark::Rigidbody2DComponent::BodyType::Kinematic:
				return b2_kinematicBody;
			}
			SK_CORE_ASSERT(false, "Unkonw body type");
			return b2_staticBody;
		}
	}


	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}
	void Scene::OnUpdateRuntime(Timestep ts)
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

		// Physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			auto view = m_Registry.view<TransformComponent,Rigidbody2DComponent>();
			for (auto&& [e,transform, rb2d] : view.each())
			{
				Entity entity = { e,this };

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}

		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 mainCameraTransform;
		auto view = m_Registry.view<TransformComponent,CameraComponent>();
		for (auto&& [entity, transfrom, camera] : view.each()) {
			if (camera.Primary) {
				mainCamera = &camera.Camera;
				mainCameraTransform = transfrom.GetTransform();
				break;
			}
		}

		if (!mainCamera)
		{
			return;
		}

		glm::mat4 viewProjection = mainCamera->GetProjection() * glm::inverse(mainCameraTransform);
		Renderer2D::BeginScene(viewProjection);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group )
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera.GetViewProjection());

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
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

	void Scene::OnRuntimeStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f,-9.807f });
		m_Registry.view<TransformComponent, Rigidbody2DComponent>().each([=](auto e, TransformComponent& transform, Rigidbody2DComponent& rb2d)
		{
			b2BodyDef bodyDef;
			bodyDef.type = Utils::ToBox2DBodyType(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;
			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			Entity entity = { e,this };
			if (auto* bc2d = entity.TryGetComponent<BoxCollider2DComponent>())
			{
				b2PolygonShape boxShape;
				boxShape.SetAsBox(transform.Scale.x * bc2d->Size.x, transform.Scale.y * bc2d->Size.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d->Density;
				fixtureDef.friction = bc2d->Friction;
				fixtureDef.restitution = bc2d->Restitution;
				fixtureDef.restitutionThreshold = bc2d->RestitutionThreshold;
				bc2d->RuntimeFixture = body->CreateFixture(&fixtureDef);
			}
		});
	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto&& [entity,camera] : view.each()) {
			if (camera.Primary) {
				return Entity { entity , this};
			}
		}
		return Entity::Empty;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create() ,this };
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		entity.AddComponent<TransformComponent>();

		return entity;
	}
	void Scene::DestoryEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
		
	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}

}

