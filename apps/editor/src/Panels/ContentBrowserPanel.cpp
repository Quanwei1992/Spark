
#include "ContentBrowserPanel.h"

#include <imgui.h>
#include <cwchar>

namespace fs = std::filesystem;

namespace Spark
{
	static const fs::path s_AssetsPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(s_AssetsPath)
	{
		m_DirectoryIcon = Texture2D::Create("resources/icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("resources/icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");
		if (m_CurrentDirectory != s_AssetsPath)
		{
			if (ImGui::Button(".."))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 64.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;


		ImGui::Columns(columnCount, 0, false);

		for (auto& it : fs::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = it.path();
			std::string pathString = path.string();
			auto relativePath = fs::relative(path, s_AssetsPath);
			auto fileName = relativePath.filename().string();

			ImGui::PushID(fileName.c_str());
			Ref<Texture2D> icon = it.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			uint64_t rendererID = (uint64_t)icon->GetRendererID();
			ImGui::ImageButton((ImTextureID)rendererID, { thumbnailSize,thumbnailSize }, { 0,1 }, { 1,0 });

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = path.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath)+1)* sizeof(wchar_t),ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}


			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if(it.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			ImGui::TextWrapped(fileName.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}


