#pragma once
#include <Spark.h>

#include "ParticleSystem.h"

class SandBox2D : public Spark::Layer
{
public:
	SandBox2D();
	virtual ~SandBox2D();
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Spark::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Spark::Event& event)override;
private:
	Spark::OrthographicCameraController m_CameraController;
	Spark::Ref<Spark::Texture2D> m_CheckerboradTexture;
	Spark::Ref<Spark::Texture2D> m_SpriteSheet;
	Spark::Ref<Spark::SubTexture2D> m_TextureStairs,m_TextureBarrel,m_TextureTree;
	float m_blueQuadRotation = 0.0f;
	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;
};