#include "Razor.h"


class ExampleLayer : public Razor::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{}

	void OnUpdate() override
	{
		if (Razor::Input::IsKeyPressed(RZ_KEY_TAB))
		{
			RZ_TRACE("Tab key is pressed!");
		}
	}

	void OnEvent(Razor::Event& event) override
	{

	}
	
};


class Sandbox : public Razor::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushLayer(new Razor::ImGuiLayer());
	}
};


Razor::Application* Razor::CreateApplication()
{
	return new Sandbox();
}


