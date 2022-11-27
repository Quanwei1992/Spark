#pragma once
#include <Razor.h>

class SandBox2D : public Razor::Layer
{
public:
	SandBox2D();
	virtual ~SandBox2D();
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Razor::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Razor::Event& event)override;
private:
	Razor::OrthographicCameraController m_CameraController;

	// Temp
	Razor::Ref<Razor::VertexArray> m_SquareVA;
	Razor::Ref<Razor::Shader> m_FlatColorShader;

	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };

};