#include "skpch.h"

#include "Spark/Renderer/Renderer.h"
#include "Spark/Renderer/RenderCommand.h"
#include "Spark/Renderer/Shader.h"
#include "Spark/Renderer/Renderer2D.h"
#include "Spark/Renderer/Framebuffer.h"
namespace Spark
{
	struct RendererData
	{
		Scope<ShaderLibrary> ShaderLib;
		Ref<RenderPass> ActiveRenderPass;
	};

	static RendererData* s_Data = nullptr;


	void Renderer::Init()
	{
		SK_PROFILE_FUNCTION();
		RenderCommand::Init();
		FramebufferPool::Init();
		//Renderer2D::Init();

		s_Data = new RendererData;
		s_Data->ShaderLib = CreateScope<ShaderLibrary>();
		s_Data->ShaderLib->Load("assets/shaders/PBR_Static.glsl");
		s_Data->ShaderLib->Load("assets/shaders/PBR_Anim.glsl");
	}

	void Renderer::Shutdown()
	{
		//Renderer2D::Shutdown();
		FramebufferPool::Shutdown();
		RenderCommand::Shutdown();
	}

	void Renderer::OnWindowResized(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0,0,width, height);
	}

	const Scope<ShaderLibrary>& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}

	void Renderer::Submit(const Ref<Shader>& shader,
		const Ref<VertexArray>& vertexArray,
		const glm::mat4& transform)
	{
		//shader->Bind();
		//shader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		//shader->SetMat4("u_Transform", transform);
		//vertexArray->Bind();
		//RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
		s_Data->ActiveRenderPass = renderPass;
		renderPass->GetSpecification().TargetFramebuffer->Bind();
	}

	void Renderer::EndRenderPass()
	{
		SK_CORE_ASSERT(s_Data->ActiveRenderPass, "No active render pass!");
		s_Data->ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
		s_Data->ActiveRenderPass = nullptr;
	}

	void Renderer::SubmitMesh(const Ref<Mesh>& mesh)
	{
	}
}
