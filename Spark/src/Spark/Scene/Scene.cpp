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

	template<typename T>
	static void CopyComponent(entt::registry& dst, entt::registry& src, std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<IDComponent,T>();
		for (auto&& [entity,id,component] : view.each())
		{
			UUID uuid = id.ID;
			dst.emplace_or_replace<T>(enttMap[uuid], component);
		}
	}

	template<typename T>
	static void CopyComponent(entt::entity dst, entt::entity src, entt::registry& registry)
	{
		T* component = registry.try_get<T>(src);
		if (component)
		{
			registry.emplace_or_replace<T>(dst, *component);
		}		
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		auto idView = srcSceneRegistry.view<IDComponent,TagComponent>();
		for (auto&& [entity,id,tag] : idView.each())
		{
			Entity entity = newScene->CreateEntityImpl(id.ID, tag.Tag);
			enttMap[id.ID] = entity;
		}

		CopyableComponentTypes::for_each([&](auto t) {
			using T = decltype(t)::type;
			CopyComponent<T>(dstSceneRegistry, srcSceneRegistry, enttMap);
		
		});
		return newScene;
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
		// Quads
		{
			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto&& [entity, transform, src] : view.each())
			{
				Renderer2D::DrawSprite(transform.GetTransform(), src, (int)entity);
			}
		}
		// Circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto&& [entity, transform, src] : view.each())
			{
				Renderer2D::DrawCircle(transform.GetTransform(), src, (int)entity);
			}
		}
		Renderer2D::EndScene();
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera.GetViewProjection());

		// Quads
		{
			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto&& [entity, transform, src] : view.each())
			{
				Renderer2D::DrawSprite(transform.GetTransform(), src, (int)entity);
			}
		}
		// Circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto&& [entity, transform, src] : view.each())
			{
				Renderer2D::DrawCircle(transform.GetTransform(), src, (int)entity);
			}
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

			if (auto* cc2d = entity.TryGetComponent<CircleCollider2DComponent>())
			{
				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d->Offset.x, cc2d->Offset.y);
				circleShape.m_radius = cc2d->Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d->Density;
				fixtureDef.friction = cc2d->Friction;
				fixtureDef.restitution = cc2d->Restitution;
				fixtureDef.restitutionThreshold = cc2d->RestitutionThreshold;
				cc2d->RuntimeFixture = body->CreateFixture(&fixtureDef);
			}
		});
	}

	void Scene::OnRuntimeStop()
	{

		m_Registry.view<Rigidbody2DComponent>().each([=](auto e, Rigidbody2DComponent& rb2d)
		{

			Entity entity = { e,this };
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			if (body)
			{
				if (auto* bc2d = entity.TryGetComponent<BoxCollider2DComponent>())
				{
					b2Fixture* fixture = (b2Fixture*)bc2d->RuntimeFixture;
					if (fixture)
					{
						body->DestroyFixture(fixture);
						bc2d->RuntimeFixture = false;
					}
				}
				if (auto* cc2d = entity.TryGetComponent<CircleCollider2DComponent>())
				{
					b2Fixture* fixture = (b2Fixture*)cc2d->RuntimeFixture;
					if (fixture)
					{
						body->DestroyFixture(fixture);
						cc2d->RuntimeFixture = false;
					}
				}
				m_PhysicsWorld->DestroyBody((b2Body*)rb2d.RuntimeBody);
				rb2d.RuntimeBody = nullptr;
			}		
		});

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

	Entity Scene::CreateEntityImpl(UUID uuid, const std::string& name)
	{

		std::string entityName = name.empty() ? "Empty" : name;


		Entity entity = { m_Registry.create() ,this };
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = entityName;
		entity.AddComponent<TransformComponent>();

		return entity;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityImpl(UUID(), name);
	}

	Spark::Entity Scene::CreateEntity(UUID uuid, const std::string& name /*= std::string()*/)
	{

		return CreateEntityImpl(uuid, name);
	}

	void Scene::DestoryEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	Spark::Entity Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName() + "_Clone");

		CopyableComponentTypes::for_each([&](auto t) {
			using T = decltype(t)::type;
			CopyComponent<T>(newEntity, entity, m_Registry);

		});

		return entity;
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(false);
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
	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{

	}
	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{

	}
}

