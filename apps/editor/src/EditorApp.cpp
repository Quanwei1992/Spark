
#include "EditorLayer.h"

#include <Spark.h>
#include <Spark/Core/EntryPoint.h>

namespace Spark
{
	class EditorApplication : public Application
	{
	public:
		EditorApplication(ApplicationCommandLineArgs args)
			:Application("Spark Editor",args)
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new EditorApplication(args);
	}
}



