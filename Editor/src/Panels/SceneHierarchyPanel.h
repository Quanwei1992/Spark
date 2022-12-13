#pragma once

#include "Spark/Core/Base.h"
#include "Spark/Scene/Scene.h"
#include "Spark/Scene/Entity.h"

namespace Spark
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity& entity);
		void DrawComponents(Entity& entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}