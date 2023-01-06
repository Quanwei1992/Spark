#include "skpch.h"
#include "SceneHierarchyPanel.h"
#include "Spark/Scene/Components.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum.hpp>

namespace Spark
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
		:m_Context(context)
	{
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = Entity::Empty;
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

		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_Context->CreateEntity("Empty Entity");
			}
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if(m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}
	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}
	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_SelectionContext == entity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				m_Context->DestoryEntity(entity);
				m_SelectionContext = Entity::Empty;
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}


	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight+3.0f,lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.15f, 0.1f,1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.15f, 0.1f, 1.0f));

		ImGui::PushFont(boldFont);

		if (ImGui::Button("X",buttonSize))
			values.x = resetValue;

		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f,0.0f,0.0f,"%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,       ImVec4(0.2f, 0.8f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.8f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f,0.9f,0.2f,1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;

		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.9f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;

		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}


	template<typename T,typename UIFunction>
	static void DrawComponent(const std::string& name,Entity entity,UIFunction uiFunction)
	{

		if (!entity.HasComponent<T>()) return;

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_FramePadding;

		ImVec2 contentRegionAvaliable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
		ImGui::Separator();
		float frameHeight = ImGui::GetFrameHeight();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegionAvaliable.x - frameHeight + 4);
		if (ImGui::Button("+", ImVec2{ frameHeight,frameHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove Component"))
			{
				removeComponent = true;
			}
			ImGui::EndPopup();
		}

		if (open)
		{
			auto& component = entity.GetComponent<T>();
			uiFunction(component);
			ImGui::TreePop();
		}
		if (removeComponent)
		{
			entity.RemoveComponent<T>();
		}

	}

	template<typename T>
	static void DrawAddComponentMenuItem(Entity entity,const char* name)
	{
		if (!entity.HasComponent<T>() && ImGui::MenuItem(name))
		{
			entity.AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}

	template<typename T>
	static bool DrawEnumComp(const char* label,T* value)
	{

		constexpr auto entries = magic_enum::enum_entries<T>();
		auto valueName = std::string(magic_enum::enum_name(*value));

		bool valueChaned = false;
		if (ImGui::BeginCombo(label, valueName.c_str()))
		{
			for (auto entrie : entries)
			{
				bool isSelected = *value == entrie.first;
				if (ImGui::Selectable(std::string(entrie.second).c_str(), isSelected))
				{
					if (*value != entrie.first)
					{
						*value = entrie.first;
						valueChaned = true;
					}
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		return valueChaned;
	}


	void SceneHierarchyPanel::DrawComponents(Entity& entity)
	{

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			DrawAddComponentMenuItem<CameraComponent>(m_SelectionContext, "Camera");
			DrawAddComponentMenuItem<SpriteRendererComponent>(m_SelectionContext, "SpriteRenderer");
			DrawAddComponentMenuItem<BoxCollider2DComponent>(m_SelectionContext, "BoxCollider2D");
			DrawAddComponentMenuItem<Rigidbody2DComponent>(m_SelectionContext, "Rigidbody2D");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();


		DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {

			DrawVec3Control("Translation", component.Translation, 0.0f, 180.0f);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation, 0.0f, 180.0f);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f, 180.0f);
		});

		DrawComponent<SpriteRendererComponent>("SpriteRenderer", entity, [](auto& component){

			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			// Texture
			ImGui::Button("Texture", ImVec2(100.0,0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
				if (payload)
				{
					std::filesystem::path path((wchar_t*)payload->Data);
					component.Texture = Texture2D::Create(path.string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Titling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f, "%.2f");
		});


		DrawComponent<Rigidbody2DComponent>("Rigidbody2D", entity, [](Rigidbody2DComponent& component) {

			DrawEnumComp<Rigidbody2DComponent::BodyType>("Type", &component.Type);
			ImGui::Checkbox("FixedRotation", &component.FixedRotation);
			
		});

		DrawComponent<BoxCollider2DComponent>("Rigidbody2D", entity, [](BoxCollider2DComponent& component) {

			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density);
			ImGui::DragFloat("Friction", &component.Friction);
			ImGui::DragFloat("Restitution", &component.Restitution);
			ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold);

		});

		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component) {
			auto& camera = component.Camera;

			ImGui::Checkbox("Primary", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

			auto projectionType = camera.GetProjectionType();

			if (DrawEnumComp<SceneCamera::ProjectionType>("Projection", &projectionType))
			{
				camera.SetProjectionType(projectionType);
			}

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
		});
	}
}
