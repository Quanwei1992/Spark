#include "skpch.h"
#include "SceneRenderer.h"

#include "Mesh.h"
#include "Spark/Scene/Scene.h"
#include "Spark/Scene/Components.h"
#include "RenderCommandQueue.h"
#include "Renderer.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Material.h"
#include "RenderPass.h"
#include "Texture.h"
#include "glad/glad.h"

namespace Spark
{

	struct SceneInfo
	{
		Camera SceneCamera;

		// Resources
		Ref<MaterialInstance> SkyboxMaterial;
		Environment SceneEnvironment;
	};

	struct DrawCommand
	{
		Ref<Mesh> Mesh;
		Ref<MaterialInstance> Material;
		glm::mat4 Transform;
	};

	struct SceneRendererData
	{
		const Scene* ActiveScene = nullptr;
		SceneInfo SceneData;
		Ref<Texture2D> BRDFLUT;
		Ref<Shader> CompositeShader;
		std::vector<DrawCommand> DrawList;
		Ref<MaterialInstance> GridMaterial;

		Ref<RenderPass> GeoPass;
		Ref<RenderPass> CompositePass;

	};

	static SceneRendererData* s_Data = nullptr;

	void SceneRenderer::Init()
	{
		s_Data = new SceneRendererData;

		// Render Passes
		FramebufferSpecification geoFrameBufferSpec;
		geoFrameBufferSpec.Width = 1920;
		geoFrameBufferSpec.Height = 1080;
		geoFrameBufferSpec.Samples = 8;
		geoFrameBufferSpec.Attachments = {
			FramebufferTextureFormat::RGBA16F,FramebufferTextureFormat::DEPTH24STENCIL8
		};
		geoFrameBufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		RenderPassSpecification geoRenderPassSpec;
		geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFrameBufferSpec);
		s_Data->GeoPass = RenderPass::Create(geoRenderPassSpec);


		FramebufferSpecification compFrameBufferSpec;
		compFrameBufferSpec.Width = 1920;
		compFrameBufferSpec.Height = 1080;
		compFrameBufferSpec.Attachments = {
			FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::DEPTH24STENCIL8
		};
		compFrameBufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };
		RenderPassSpecification compRenderPassSpec;
		compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFrameBufferSpec);
		s_Data->CompositePass = RenderPass::Create(compRenderPassSpec);

		s_Data->CompositeShader = Shader::Create("assets/shaders/SceneComposite.glsl");
		s_Data->BRDFLUT = Texture2D::Create("assets/textures/BRDF_LUT.tga");

		// Grid
		auto gridShader = Shader::Create("assets/shaders/Grid.glsl");
		s_Data->GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
		s_Data->GridMaterial->Set("u_Scale", 16.025f);
		s_Data->GridMaterial->Set("u_Res", 0.025f);

	}

	void SceneRenderer::Shutdown()
	{
		delete s_Data;
		s_Data = nullptr;
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		s_Data->GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
		s_Data->CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
	}

	void SceneRenderer::BeginScene(Scene* scene,Camera camera)
	{
		SK_CORE_ASSERT(s_Data->ActiveScene == nullptr);
		s_Data->ActiveScene = scene;
		s_Data->SceneData.SceneCamera = camera;
		s_Data->SceneData.SkyboxMaterial = scene->m_SkyboxMaterial;
		s_Data->SceneData.SceneEnvironment = scene->m_Environment;
	}

	void SceneRenderer::EndScene()
	{
		SK_CORE_ASSERT(s_Data->ActiveScene);
		s_Data->ActiveScene = nullptr;
		FlushDrawList();
	}

	void SceneRenderer::SubmitEntity(Entity entity)
	{
		auto meshRenderer = entity.GetComponent<MeshRendererComponent>();
		auto transform = entity.GetComponent<TransformComponent>();
		if(meshRenderer.Mesh)
		{
			s_Data->DrawList.push_back({ meshRenderer.Mesh,meshRenderer.Material,transform.GetTransform() });
		}
	}

	static Ref<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

	Environment SceneRenderer::CreateEnvironmentMap(const std::string& filepath)
	{
		const uint32_t cubemapSize = 2048;
		const uint32_t irradianceMapSize = 32;

		Ref<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::RGBA16F, cubemapSize, cubemapSize);
		if (!equirectangularConversionShader)
			equirectangularConversionShader = Shader::Create("assets/shaders/EquirectangularToCubeMap.glsl");

		Ref<Texture2D> envEquirect = Texture2D::Create(filepath);
		SK_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::RGBA16F, "Texture is not HDR!");

		equirectangularConversionShader->Bind();
		envEquirect->Bind();

		RenderCommandQueue::Submit([envUnfiltered, cubemapSize]()
		{
			glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
			glGenerateTextureMipmap(envUnfiltered->GetRendererID());
		});

		if (!envFilteringShader)
			envFilteringShader = Shader::Create("assets/shaders/EnvironmentMipFilter.glsl");

		Ref<TextureCube> envFiltered = TextureCube::Create(TextureFormat::RGBA16F, cubemapSize, cubemapSize);

		RenderCommandQueue::Submit([envUnfiltered, envFiltered]()
		{
			glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			envFiltered->GetWidth(), envFiltered->GetHeight(), 6);
		});

		envFilteringShader->Bind();
		envUnfiltered->Bind();
		RenderCommandQueue::Submit([envUnfiltered, envFiltered, cubemapSize]() {
			const float deltaRoughness = 1.0f / glm::max((float)(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
			for (int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
			{
				const GLuint numGroups = glm::max(1, size / 32);
				glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
		});

		if (!envIrradianceShader)
			envIrradianceShader = Shader::Create("assets/shaders/EnvironmentIrradiance.glsl");

		Ref<TextureCube> irradianceMap = TextureCube::Create(TextureFormat::RGBA16F, irradianceMapSize, irradianceMapSize);
		envIrradianceShader->Bind();
		envFiltered->Bind();
		RenderCommandQueue::Submit([irradianceMap]()
		{
			glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
		});

		return { envFiltered, irradianceMap,envEquirect };
	}



	void SceneRenderer::GeometryPass()
	{
		Renderer::BeginRenderPass(s_Data->GeoPass);
		

		auto viewProjection = s_Data->SceneData.SceneCamera.GetProjectionMatrix() * s_Data->SceneData.SceneCamera.GetViewMatrix();

		// Skybox
		auto skyBoxShader = s_Data->SceneData.SkyboxMaterial->GetShader();
		s_Data->SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
		Renderer::SubmitFullScreenQuad(s_Data->SceneData.SkyboxMaterial);

		// Render entities
		for (auto& dc : s_Data->DrawList)
		{
			auto baseMaterial = dc.Mesh->GetMaterial();
			baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
			baseMaterial->Set("u_CameraPosition", s_Data->SceneData.SceneCamera.GetPosition());

			// Environment (TODO: don't do this per mesh)
			baseMaterial->Set("u_EnvRadianceTex", s_Data->SceneData.SceneEnvironment.RadianceMap);
			baseMaterial->Set("u_EnvIrradianceTex", s_Data->SceneData.SceneEnvironment.IrradianceMap);
			baseMaterial->Set("u_BRDFLUTTexture", s_Data->BRDFLUT);

			auto overrideMaterial = nullptr; // dc.Material;
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
		}

		// Grid
		s_Data->GridMaterial->Set("u_ViewProjection", viewProjection);
		Renderer::SubmitQuad(s_Data->GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));

		Renderer::EndRenderPass();
	}

	void SceneRenderer::CompositePass()
	{
		Renderer::BeginRenderPass(s_Data->CompositePass);
		s_Data->CompositeShader->Bind();
		s_Data->CompositeShader->SetFloat("u_Exposure", s_Data->SceneData.SceneCamera.GetExposure());
		s_Data->CompositeShader->SetInt("u_TextureSamples", s_Data->GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
		s_Data->GeoPass->GetSpecification().TargetFramebuffer->BindTexture(0);
		Renderer::SubmitFullScreenQuad(nullptr);
		Renderer::EndRenderPass();
	}

	Ref<Texture2D> SceneRenderer::GetFinalColorBuffer()
	{
		SK_CORE_ASSERT(false, "Not implemented");
		return nullptr;
	}

	uint32_t SceneRenderer::GetFinalColorBufferRendererID()
	{
		return s_Data->CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
	}

	void SceneRenderer::FlushDrawList()
	{
		SK_CORE_ASSERT(!s_Data->ActiveScene);

		GeometryPass();
		CompositePass();

		s_Data->DrawList.clear();
		s_Data->SceneData = {};
	}
}
