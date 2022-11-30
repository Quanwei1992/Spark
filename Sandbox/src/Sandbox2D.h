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
	Razor::Ref<Razor::Texture2D> m_CheckerboradTexture;
	float m_blueQuadRotation = 0.0f;
};