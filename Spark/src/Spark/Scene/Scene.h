#pragma once

#include "entt.hpp"

namespace Spark
{
	class Scene
	{
	public:
		Scene();
		~Scene();

	private:
		entt::registry m_Registry;
	};
}