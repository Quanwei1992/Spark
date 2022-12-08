
#include "EditorLayer.h"

#include <Spark.h>
#include <Spark/Core/EntryPoint.h>

namespace Spark
{
	class SparkEditor : public Application
	{
	public:
		SparkEditor()
			:Application("Spark Editor")
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication()
	{
		return new SparkEditor();
	}
}



