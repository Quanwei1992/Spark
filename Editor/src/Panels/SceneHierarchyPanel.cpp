#include "skpch.h"
#include "SceneHierarchyPanel.h"
#include "Spark/Scene/Components.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Spark
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
		:m_Context(context)
	{
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([this](auto e)
		{
			Entity entity = {e,m_Context.get()};
			DrawEntityNode(entity);

		});		


		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if(m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}
	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_SelectionContext == entity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}
	void SceneHierarchyPanel::DrawComponents(Entity& entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}
	
		if (entity.HasComponent<TransformComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& transform = entity.GetComponent<TransformComponent>().Transform;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);
				ImGui::TreePop();
			}

		}


		if (entity.HasComponent<SpriteRendererComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{
				auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(spriteRenderer.Color));
				ImGui::TreePop();
			}

		}


		if (entity.HasComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;


				ImGui::Checkbox("Primary", &cameraComponent.Primary);
				ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);


				const char* projectionTypeStrings[] = { "Perspective","Orthograpic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (size_t i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				auto projectionType = camera.GetProjectionType();
				if (projectionType == SceneCamera::ProjectionType::Perspective)
				{
					float fov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &fov))
					{
						camera.SetPerspectiveVerticalFOV(glm::radians(fov));
					}

					float nearClip = camera.GetPerspectiveNear();
					if (ImGui::DragFloat("Near", &nearClip))
					{
						camera.SetPerspectiveNear(nearClip);
					}

					float farClip = camera.GetPerspectiveFar();
					if (ImGui::DragFloat("Far", &farClip))
					{
						camera.SetPerspectiveFar(farClip);
					}
				}
				else if (projectionType == SceneCamera::ProjectionType::Otrhographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
					{
						camera.SetOrthographicSize(orthoSize);
					}
					
					float nearClip = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &nearClip))
					{
						camera.SetOrthographicNearClip(nearClip);
					}

					float farClip = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &farClip))
					{
						camera.SetOrthographicFarClip(farClip);
					}

				}

				ImGui::TreePop();
			}

		}
	}
}