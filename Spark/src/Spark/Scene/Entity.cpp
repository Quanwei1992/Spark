#include "skpch.h"
#include "Entity.h"
#include "Components.h"

namespace Spark
{

	UUID Entity::GetUUID()
	{
		return GetComponent<IDComponent>().ID;
	}

	const std::string& Entity::GetName()
	{
		return GetComponent<TagComponent>().Tag;
	}

	Entity Entity::Empty = {};

	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityHandle(handle)
		,m_Scene(scene)
	{

	}
}
