#include "skpch.h"
#include "Entity.h"
#include "Components.h"

namespace Spark
{

	UUID Entity::GetUUID()
	{
		return GetComponent<IDComponent>().ID;
	}

	Entity Entity::Empty = {};

	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityHandle(handle)
		,m_Scene(scene)
	{

	}
}
