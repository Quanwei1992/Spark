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

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
 
	private:
		void DrawEntityNode(Entity& entity);
		void DrawComponents(Entity& entity);


		template<typename T>
		void DrawComponent(Entity entity);

		template<typename T>
		void OnDrawComponentInspectorGUI(Entity entity, T& component);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};


}
