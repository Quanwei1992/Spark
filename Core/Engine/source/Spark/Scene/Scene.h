#pragma once

#include "entt.hpp"

#include "Spark/Core/Timestep.h"
#include "Spark/Core/UUID.h"
#include "Spark/Renderer/EditorCamera.h"

class b2World;

namespace Spark
{
	class Texture2D;
	class MaterialInstance;
	class TextureCube;
	class Entity;
	class SceneSerializer;

	struct Environment
	{
		Ref<TextureCube> RadianceMap;
		Ref<TextureCube> IrradianceMap;
		Ref<Texture2D> Equirect;

		static Environment Load(const std::string& filepath);
	};

	class Scene
	{
	public:
		Scene(const std::string& debugName="scene");
		~Scene();

		void Init();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntity(UUID uuid,const std::string& name = std::string());
		void DestoryEntity(Entity entity);

		Entity DuplicateEntity(Entity entity);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);
		void OnRuntimeStart();
		void OnRuntimeStop();
		Entity GetPrimaryCameraEntity();

		void SetEnvironment(const Environment& Environment);
		void SetSkybox(const Ref<TextureCube>& skybox);

		float& GetSkyBoxLod() { return m_SkyboxLod; }

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		void OnPhysics2DStart();
		void OnPhysics2DStop();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
		Entity CreateEntityImpl(UUID uuid, const std::string& name);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth;
		uint32_t m_ViewportHeight;
		b2World* m_PhysicsWorld = nullptr;
		std::string m_DebugName;
		Environment m_Environment;
		Ref<TextureCube> m_SkyboxTexture;
		Ref<MaterialInstance> m_SkyboxMaterial;

		float m_SkyboxLod = 1.0f;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
		friend class SceneRenderer;
	};

}
