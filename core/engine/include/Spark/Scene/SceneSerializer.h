#pragma once

#include "Scene.h"

namespace Spark
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filePath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filePath);

	private:
		Ref<Scene> m_Scene;
	};
}