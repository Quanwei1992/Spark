#include "Razor.h"


class ExampleLayer : public Razor::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{}

	void OnUpdate() override
	{
		RZ_INFO("Examplelayer::Update");
	}

	void OnEvent(Razor::Event& event) override
	{
		RZ_TRACE("{0}", event);
	}
	
};


class Sandbox : public Razor::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
};


Razor::Application* Razor::CreateApplication()
{
	return new Sandbox();
}


