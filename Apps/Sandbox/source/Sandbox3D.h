#pragma once
#include <Spark.h>


using Texture2DRef = Spark::Ref<Spark::Texture2D>;

struct AlbedoInput
{
	glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; // Sliver
	Texture2DRef TextureMap;
	bool SRGB = true;
	bool UseTexture = false;
};

struct NormalInput
{
	Texture2DRef TextureMap;
	bool UseTexture = false;
};

struct MetalnessInput
{
	float Value = 1.0f;
	Texture2DRef TextureMap;
	bool UseTexture = false;
};

struct RoughnessInput
{
	float Value = 0.5f;
	Texture2DRef TextureMap;
	bool UseTexture;
};

struct Light
{
	glm::vec3 Direction;
	glm::vec3 Radiance;
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
	bool OnKeyPressed(Spark::KeyPressedEvent& e);
private:
	Spark::Ref<Spark::Shader> m_QuadShader;
	Spark::Ref<Spark::Shader> m_HDRShader;
	Spark::Ref<Spark::Shader> m_GridShader;
	Spark::Ref<Spark::Mesh> m_Mesh;
	Spark::Ref<Spark::Mesh> m_SphereMesh, m_PlaneMesh;
	Spark::Ref<Spark::Texture2D> m_BRDFLUT;

	Spark::Ref<Spark::MaterialInstance> m_MeshMaterial;
	Spark::Ref<Spark::MaterialInstance> m_GridMaterial;
	std::vector<Spark::Ref<Spark::MaterialInstance>> m_MetalSphereMaterialInstances;
	std::vector<Spark::Ref<Spark::MaterialInstance>> m_DielectricSphereMaterialInstances;

	float m_GridScale = 16.025f, m_GridSize = 0.025;
	float m_MeshScale = 1.0f;

	AlbedoInput m_AlbedoInput;
	NormalInput m_NormalInput;
	MetalnessInput m_MetalnessInput;
	RoughnessInput m_RoughnessInput;

	Spark::Ref<Spark::RenderPass> m_GeoPass, m_CompositePass;

	Spark::Ref<Spark::VertexArray> m_QuadVertexArray;
	Spark::Ref<Spark::TextureCube> m_EnvironmentCubeMap, m_EnvironmentIrradiance;

	Light m_Light;
	float m_LightMultiplier = 0.3f;

	// PBR params
	float m_Exposure = 1.0f;
	bool m_RadiancePrefilter = false;
	float m_EnvMapRotation = 0.0f;

	enum class SceneType : uint32_t
	{
		Spheres =0,Model =1
	};
	SceneType m_SceneType;

	Spark::Ref<Spark::Texture2D> m_CheckerboradTex;
	Spark::Camera m_Camera;

	int m_GizmoType = -1; // no gizmo
	glm::mat4 m_Transform;
};
