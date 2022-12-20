#include "skpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>



namespace YAML
{
	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator << (YAML::Emitter& out, const Spark::Color4f& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Spark::Color4f> {
		static Node encode(const Spark::Color4f& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, Spark::Color4f& rhs) {
			if (!node.IsSequence() || node.size() != 4) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
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
		out << YAML::Key << "Entity" << YAML::Value << "123456789"; // TODO: UUID


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

			Entity deserializedEntity = m_Scene->CreateEntity(name);

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
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		SK_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}


