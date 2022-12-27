#pragma once
#include "Spark/Core/Base.h"
#include "Spark/Scene/Scene.h"
#include "Spark/Scene/Entity.h"

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
	};
}
