#pragma once


#include "SceneCamera.h"
#include "Spark/Renderer/Camera.h"
#include "Spark/Core/Timestep.h"
#include "Spark/Scene/ScriptableEntity.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Spark
{

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f,0.0f,0.0f };
		glm::vec3 Rotation = { 0.0f,0.0f,0.0f };
		glm::vec3 Scale = { 1.0f,1.0f,1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}


		glm::mat4 GetTransform() const
		{

			const auto& rotation = glm::toMat4(glm::quat(Rotation));

			const auto& scale = glm::scale(glm::mat4(1.0f), Scale);

			const auto& translation = glm::translate(glm::mat4(1.0f), Translation);

			return translation * rotation * scale;
			
		}
	};

	struct SpriteRendererComponent
	{
		Color4f Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const Color4f& color)
			: Color(color) {}
	};
	

	struct CameraComponent
	{
		Spark::SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;
		ScriptableEntity*(*InstantiateScript)();
		void (*DestoryScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestoryScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };
		}

	};
}