#include "Sandbox2D.h"
#include "Sandbox3D.h"

#include <Spark.h>
#include <Spark/Core/EntryPoint.h>
#include <imgui.h>


class Sandbox : public Spark::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		//PushLayer(new SandBox2D());
		PushLayer(new SandBox3D());
	}
};


Spark::Application* Spark::CreateApplication(Spark::ApplicationCommandLineArgs args)
{
	return new Sandbox();
}


