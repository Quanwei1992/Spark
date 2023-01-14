#include "skpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>
#include <magic_enum.hpp>


namespace YAML
{
	template<int N>
	YAML::Emitter& operator << (YAML::Emitter& out, glm::vec<N, float, glm::defaultp>& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		
		for (int i = 0; i < N; i++)
		{
			out << v[i];
		}
		out << YAML::EndSeq;
		return out;
	}

	template<int N>
	struct convert<glm::vec<N,float, glm::defaultp>> {
		static Node encode(const glm::vec<N, float, glm::defaultp>& rhs) {
			Node node;
			for (auto value : rhs)
			{
				node.push_back(value);
			}
			return node;
		}


		static bool decode(const Node& node, glm::vec<N, float, glm::defaultp>& rhs) {
			if (!node.IsSequence() || node.size() != N) {
				return false;
			}

			for (int i= 0;i<N;i++)
			{
				rhs[i] = node[i].as<float>();
			}
			return true;
		}
	};

}

namespace Spark
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		:m_Scene(scene)
	{
	}

	static void SeralizeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << (uint64_t)entity.GetUUID(); // TODO: UUID


		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent" << YAML::Value << YAML::BeginMap;

			auto& component = entity.GetComponent<TagComponent>();
			out << YAML::Key << "Tag" << YAML::Value << component.Tag;

			out << YAML::EndMap;
		}
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent" << YAML::Value << YAML::BeginMap;

			auto& component = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << component.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << component.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << component.Scale;

			out << YAML::EndMap;
		}
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent" << YAML::Value << YAML::BeginMap;
			auto& component = entity.GetComponent<CameraComponent>();
			auto& camera = component.Camera;

			out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();

			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNear();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFar();

			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << component.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << component.FixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent" << YAML::Value << YAML::BeginMap;

			auto& component = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << component.Color;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent" << YAML::Value << YAML::BeginMap;

			auto& component = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << component.Color;
			out << YAML::Key << "Thickness" << YAML::Value << component.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << component.Fade;
			out << YAML::EndMap;
		}

		if (Rigidbody2DComponent* component = entity.TryGetComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Type" << YAML::Value << std::string(magic_enum::enum_name(component->Type));
			out << YAML::Key << "FixedRotation" << YAML::Value << component->FixedRotation;
			out << YAML::EndMap;
		}
		if (BoxCollider2DComponent* component = entity.TryGetComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Offset" << YAML::Value << component->Offset;
			out << YAML::Key << "Size" << YAML::Value << component->Size;
			out << YAML::Key << "Density" << YAML::Value << component->Density;
			out << YAML::Key << "Friction" << YAML::Value << component->Restitution;
			out << YAML::Key << "Restitution" << YAML::Value << component->Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << component->RestitutionThreshold;
			out << YAML::EndMap;
		}
		if (CircleCollider2DComponent* component = entity.TryGetComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Offset" << YAML::Value << component->Offset;
			out << YAML::Key << "Radius" << YAML::Value << component->Radius;
			out << YAML::Key << "Density" << YAML::Value << component->Density;
			out << YAML::Key << "Friction" << YAML::Value << component->Restitution;
			out << YAML::Key << "Restitution" << YAML::Value << component->Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << component->RestitutionThreshold;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = Entity{ entityID,m_Scene.get() };
			if (!entity) return;
			SeralizeEntity(out,entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.flush();
		fout.close();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filePath)
	{
		SK_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{

		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		SK_CORE_TRACE("Deserializing scene '{0}'", sceneName);


		auto entities = data["Entities"];

		if (!entities) return false;

		for (auto entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO: UUID

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent) {
				name = tagComponent["Tag"].as<std::string>();
			}

			SK_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = m_Scene->CreateEntity(uuid,name);

			auto componentData = entity["TransformComponent"];
			if (componentData)
			{
				auto& c = deserializedEntity.GetComponent<TransformComponent>();
				c.Translation = componentData["Translation"].as<glm::vec3>();
				c.Rotation = componentData["Rotation"].as<glm::vec3>();
				c.Scale = componentData["Scale"].as<glm::vec3>();
			}

			componentData = entity["CameraComponent"];
			if (componentData)
			{

				auto& c = deserializedEntity.AddComponent<CameraComponent>();

				auto cameraData = componentData["Camera"];

				c.Camera.SetProjectionType((Spark::SceneCamera::ProjectionType)cameraData["ProjectionType"].as<int>());

				c.Camera.SetOrthographicSize(cameraData["OrthographicSize"].as<float>());
				c.Camera.SetOrthographicNearClip(cameraData["OrthographicNear"].as<float>());
				c.Camera.SetOrthographicFarClip(cameraData["OrthographicFar"].as<float>());

				c.Camera.SetPerspectiveVerticalFOV(cameraData["PerspectiveFOV"].as<float>());
				c.Camera.SetPerspectiveNear(cameraData["PerspectiveNear"].as<float>());
				c.Camera.SetPerspectiveFar(cameraData["PerspectiveFar"].as<float>());

				c.Primary = componentData["Primary"].as<bool>();
				c.FixedAspectRatio = componentData["FixedAspectRatio"].as<bool>();

			}

			componentData = entity["SpriteRendererComponent"];
			if (componentData)
			{
				auto& c = deserializedEntity.AddComponent<SpriteRendererComponent>();
				c.Color = componentData["Color"].as<Color4f>();
			}

			componentData = entity["CircleRendererComponent"];
			if (componentData)
			{
				auto& c = deserializedEntity.AddComponent<CircleRendererComponent>();
				c.Color = componentData["Color"].as<Color4f>();
				c.Thickness = componentData["Thickness"].as<float>();
				c.Fade = componentData["Fade"].as<float>();
			}

			componentData = entity["Rigidbody2DComponent"];
			if (componentData)
			{
				auto& c = deserializedEntity.AddComponent<Rigidbody2DComponent>();
				std::string bodyTypeName = componentData["Type"].as<std::string>();
				auto bodyType = magic_enum::enum_cast<Rigidbody2DComponent::BodyType>(bodyTypeName);
				if (bodyType.has_value()) {
					c.Type = bodyType.value();
				}
				else {
					SK_CORE_WARN("Unkonw Rigidbody2DComponent::Type {0}", bodyTypeName);
				}

				c.FixedRotation = componentData["FixedRotation"].as<bool>();		
			}

			componentData = entity["BoxCollider2DComponent"];
			if (componentData)
			{
				auto& c = deserializedEntity.AddComponent<BoxCollider2DComponent>();
				c.Offset = componentData["Offset"].as<glm::vec2>();
				c.Size = componentData["Size"].as<glm::vec2>();
				c.Density = componentData["Density"].as<float>();
				c.Friction = componentData["Friction"].as<float>();
				c.Restitution = componentData["Restitution"].as<float>();
				c.RestitutionThreshold = componentData["RestitutionThreshold"].as<float>();
			}

			componentData = entity["CircleCollider2DComponent"];
			if (componentData)
			{
				auto& c = deserializedEntity.AddComponent<CircleCollider2DComponent>();
				c.Offset = componentData["Offset"].as<glm::vec2>();
				c.Radius = componentData["Radius"].as<float>();
				c.Density = componentData["Density"].as<float>();
				c.Friction = componentData["Friction"].as<float>();
				c.Restitution = componentData["Restitution"].as<float>();
				c.RestitutionThreshold = componentData["RestitutionThreshold"].as<float>();
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		SK_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}


