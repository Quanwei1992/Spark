#include "skpch.h"
#include "ScriptGlue.h"
#include "Spark/Scene/Scene.h"
#include "Spark/Scripting/ScriptEngine.h"
#include "Spark/Scene/Entity.h"
#include "Spark/Scene/Components.h"
#include "Spark/Events/Input.h"

#include <mono/metadata/loader.h>
#include <mono/metadata/object.h>



namespace Spark
{

#define SK_ADD_INTERNAL_CALL(NAME) mono_add_internal_call("Spark.InternalCalls::"#NAME, NAME)

	static void Entity_GetTranslation(uint32_t entityID,glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = { static_cast<entt::entity>(entityID),scene };

		auto& component = entity.GetComponent<TransformComponent>();
		*outTranslation = component.Translation;
	}
	static void Entity_SetTranslation(uint32_t entityID,glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = { static_cast<entt::entity>(entityID),scene };

		auto& component = entity.GetComponent<TransformComponent>();
		component.Translation = *translation;
	}

	static bool Input_IsKeyPressed(int keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	static bool Input_IsMouseButtonPressed(int button)
	{
		return Input::IsMouseButtonPressed(button);
	}

	static void Input_GetMousePosition(float* x,float*y)
	{
		auto [mouseX,mouseY] = Input::GetMousePosition();
		*x = mouseX;
		*y = mouseY;
	}


	void ScriptGlue::RegisterFunctions()
	{
		SK_ADD_INTERNAL_CALL(Entity_GetTranslation);
		SK_ADD_INTERNAL_CALL(Entity_SetTranslation);
		SK_ADD_INTERNAL_CALL(Input_IsKeyPressed);
		SK_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
		SK_ADD_INTERNAL_CALL(Input_GetMousePosition);
	}
}
