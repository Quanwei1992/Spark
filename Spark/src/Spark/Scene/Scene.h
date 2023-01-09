#pragma once

#include "entt.hpp"

#include "Spark/Core/Timestep.h"
#include "Spark/Core/UUID.h"
#include "Spark/Renderer/EditorCamera.h"

class b2World;

namespace Spark
{
	class Entity;
	class SceneSerializer;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntity(UUID uuid,const std::string& name = std::string());
		void DestoryEntity(Entity entity);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);
		void OnRuntimeStart();
		void OnRuntimeStop();
		Entity GetPrimaryCameraEntity();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		Entity CreateEntityImpl(UUID uuid, const std::string& name);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth;
		uint32_t m_ViewportHeight;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

}
