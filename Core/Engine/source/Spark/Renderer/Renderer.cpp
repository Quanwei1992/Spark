#include "skpch.h"

#include "Spark/Renderer/Renderer.h"

#include <assimp/scene.h>

#include "RenderCommandQueue.h"
#include "Spark/Renderer/RenderCommand.h"
#include "Spark/Renderer/Shader.h"
#include "Spark/Renderer/Renderer2D.h"
#include "Spark/Renderer/Framebuffer.h"
#include "Spark/Renderer/Material.h"

#include <glad/glad.h>

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
		RenderCommandQueue::Init();
		RenderCommand::Init();
		FramebufferPool::Init();
		//Renderer2D::Init();

		s_Data = new RendererData;
		s_Data->ShaderLib = CreateScope<ShaderLibrary>();
		s_Data->ShaderLib->Load("assets/shaders/PBR_Static.glsl");
		s_Data->ShaderLib->Load("assets/shaders/PBR_Anim.glsl");

		RenderCommandQueue::Execute();
	}

	void Renderer::Shutdown()
	{
		//Renderer2D::Shutdown();
		FramebufferPool::Shutdown();
		RenderCommand::Shutdown();
		RenderCommandQueue::Shutdown();
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

	void Renderer::SubmitMesh(const Ref<Mesh>& mesh, const glm::mat4& transform, Ref<MaterialInstance> material)
	{
		if (material)
		{
			material->Bind();
		}

		bool materialOverride = !!material;

		mesh->m_VertexArray->Bind();

		for (Submesh& submesh : mesh->m_Submeshes)
		{

			if (mesh->m_Scene->mAnimations)
			{
				for (size_t i = 0; i < mesh->m_BoneTransforms.size(); i++)
				{
					std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
					mesh->m_MeshShader->SetMat4(uniformName, mesh->m_BoneTransforms[i]);
				}
			}
			if (!materialOverride)
			{
				mesh->m_MeshShader->SetMat4("u_ModelMatrix", transform * submesh.Transform);
			}
			RenderCommandQueue::Submit([submesh]()
				{
					glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
				});

		}
		mesh->m_VertexArray->Unbind();
	}
}
