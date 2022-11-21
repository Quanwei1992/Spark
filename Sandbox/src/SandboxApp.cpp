#include "Razor.h"

#include <imgui.h>

class ExampleLayer : public Razor::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{

	}

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

	virtual void OnImGuiRender()
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello world");
		ImGui::End();
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


