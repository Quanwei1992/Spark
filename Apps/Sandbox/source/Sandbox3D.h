#pragma once
#include <Spark.h>

#include "ParticleSystem.h"

struct CameraData
{
	glm::mat4 ViewProjection;
};


class SandBox3D : public Spark::Layer
{
public:
	SandBox3D();
	virtual ~SandBox3D();
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Spark::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Spark::Event& event)override;
private:
	bool OnWindowResized(Spark::WindowResizeEvent& e);
private:
	Spark::Ref<Spark::Shader> m_SimplePBRShader;
	Spark::Ref<Spark::Shader> m_QuadShader;
	Spark::Ref<Spark::Shader> m_HDRShader;
	Spark::Ref<Spark::Shader> m_GridShader;

	Spark::Ref<Spark::Mesh> m_SphereMesh, m_PlaneMesh;
	
	Spark::Ref<Spark::Texture2D> m_BRDFLUT;
	Spark::Ref<Spark::Texture2D> m_CheckerboradTexture;

	Spark::Ref<Spark::Mesh> m_CubeMesh;
	Spark::EditorCamera m_EditorCamera;
	Spark::Ref<Spark::UniformBuffer> m_CameraUniformBuffer;
	CameraData m_CameraData;
};
