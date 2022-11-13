#include "Razor.h"


class ExampleLayer : public Razor::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{}

	void OnUpdate() override
	{

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


