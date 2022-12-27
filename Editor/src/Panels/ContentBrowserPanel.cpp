#include "skpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>


namespace fs = std::filesystem;

namespace Spark
{
	static const fs::path s_AssetsPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(s_AssetsPath)
	{
	
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

		for (auto& it : fs::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = it.path();
			std::string pathString = path.string();
			auto relativePath = fs::relative(path, s_AssetsPath);
			auto fileName = relativePath.filename().string();
			if (it.is_directory())
			{
				if (ImGui::Button(fileName.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else {
				if (ImGui::Button(fileName.c_str()))
				{

				}
			}
		}

		ImGui::End();
	}
}


