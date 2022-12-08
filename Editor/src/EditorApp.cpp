
#include "EditorLayer.h"

#include <Spark.h>
#include <Spark/Core/EntryPoint.h>

namespace Spark
{
	class EditorApplication : public Application
	{
	public:
		EditorApplication()
			:Application("Spark Editor")
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication()
	{
		return new EditorApplication();
	}
}



