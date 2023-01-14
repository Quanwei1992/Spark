#pragma once


#include "SceneCamera.h"
#include "Spark/Renderer/Camera.h"
#include "Spark/Core/Timestep.h"
#include "Spark/Core/UUID.h"
#include "Spark/Scene/ScriptableEntity.h"
#include "Spark/Renderer/Texture.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <typelist.hpp>
#include <typeindex>

namespace Spark
{

	struct IDComponent
	{
		constexpr static const char* PrettyName = "ID";

		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& id)
			: ID(id) {}
	};

	struct TagComponent
	{
		constexpr static const char* PrettyName = "Tag";

		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		constexpr static const char* PrettyName = "Transform";


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

		constexpr static const char* PrettyName = "SpriteRenderer";

		Color4f Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const Color4f& color)
			: Color(color) {}
	};


	struct CircleRendererComponent
	{
		constexpr static const char* PrettyName = "CircleRenderer";

		Color4f Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};
	
	struct CameraComponent
	{
		constexpr static const char* PrettyName = "Camera";

		Spark::SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		constexpr static const char* PrettyName = "NativeScript";

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

	// Physics
	struct Rigidbody2DComponent
	{
		constexpr static const char* PrettyName = "Rigidbody2D";

		enum class BodyType { Static = 0,Dynamic,Kinematic};
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		constexpr static const char* PrettyName = "BoxCollider2D";

		glm::vec2 Offset = { 0.0f,0.0f };
		glm::vec2 Size = { 0.5f,0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;
	
		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};


	struct CircleCollider2DComponent
	{
		constexpr static const char* PrettyName = "CircleCollider2D";

		glm::vec2 Offset = { 0.0f,0.0f };
		float Radius = 1.0f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;


		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};


	using CopyableComponentTypes = tl::type_list<TransformComponent
		, SpriteRendererComponent
		, CameraComponent
		, NativeScriptComponent
		, Rigidbody2DComponent
		, BoxCollider2DComponent
		, CircleCollider2DComponent
		, CircleRendererComponent
	>;
}
