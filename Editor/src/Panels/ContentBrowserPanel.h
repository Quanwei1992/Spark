#pragma once
#include "Spark/Core/Base.h"
#include "Spark/Scene/Scene.h"
#include "Spark/Scene/Entity.h"
#include "Spark/Renderer/Texture.h"

#include <filesystem>

namespace Spark
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}
