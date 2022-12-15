#include "skpch.h"
#include "Entity.h"

namespace Spark
{
	Entity Entity::Empty = {};

	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityHandle(handle)
		,m_Scene(scene)
	{

	}
}