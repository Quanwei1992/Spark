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

#include "SceneRenderer.h"

namespace Spark
{
	struct RendererData
	{
		Ref<RenderPass> ActiveRenderPass;
		Scope<ShaderLibrary> ShaderLib;
		Ref<VertexArray> QuadVertexArray;
	};

	static RendererData* s_Data = nullptr;


	void Renderer::Init()
	{
		SK_PROFILE_FUNCTION();
		RenderCommandQueue::Init();
		RenderCommand::Init();
		FramebufferPool::Init();
		//Renderer2D::Init();
		SceneRenderer::Init();

		s_Data = new RendererData;
		s_Data->ShaderLib = CreateScope<ShaderLibrary>();
		s_Data->ShaderLib->Load("assets/shaders/PBR_Static.glsl");
		s_Data->ShaderLib->Load("assets/shaders/PBR_Anim.glsl");

		// Full Screen Quad
		float x = -1,y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};
		QuadVertex* data = new QuadVertex[4];
		data[0].Position = glm::vec3(x, y, 0);
		data[0].TexCoord = glm::vec2(0, 0);
		data[1].Position = glm::vec3(x + width, y, 0);
		data[1].TexCoord = glm::vec2(1, 0);
		data[2].Position = glm::vec3(x + width, y + height, 0);
		data[2].TexCoord = glm::vec2(1, 1);
		data[3].Position = glm::vec3(x, y + height, 0);
		data[3].TexCoord = glm::vec2(0, 1);

		s_Data->QuadVertexArray = VertexArray::Create();
		auto quadVB = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
		quadVB->SetLayout({
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float2,"a_TexCoord"}
		});
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		auto quadIB = IndexBuffer::Create(indices, sizeof(uint32_t) * 6);

		s_Data->QuadVertexArray->AddVertexBuffer(quadVB);
		s_Data->QuadVertexArray->SetIndexBuffer(quadIB);

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

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass,bool clear)
	{
		s_Data->ActiveRenderPass = renderPass;
		renderPass->GetSpecification().TargetFramebuffer->Bind();
		if(clear)
		{
			const glm::vec4& clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
			RenderCommand::SetClearColor(clearColor);
			RenderCommand::Clear();
		}
	}

	void Renderer::EndRenderPass()
	{
		SK_CORE_ASSERT(s_Data->ActiveRenderPass, "No active render pass!");
		s_Data->ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
		s_Data->ActiveRenderPass = nullptr;
	}

	void Renderer::SubmitMesh(const Ref<Mesh>& mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial)
	{

		mesh->m_VertexArray->Bind();
		auto& materials = mesh->GetMaterials();
		for (Submesh& submesh : mesh->m_Submeshes)
		{
			// Material
			auto material = materials[submesh.MaterialIndex];
			auto shader = material->GetShader();
			material->Bind();

			if (mesh->m_IsAnimated)
			{
				for (size_t i = 0; i < mesh->m_BoneTransforms.size(); i++)
				{
					std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
					mesh->m_MeshShader->SetMat4(uniformName, mesh->m_BoneTransforms[i]);
				}
			}

			shader->SetMat4("u_Transform", transform * submesh.Transform);
			RenderCommand::EnbaleDepthTest(material->HasFlag(MaterialFlag::DepthTest));
			RenderCommandQueue::Submit([submesh,material]()
			{
				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
			});

		}
		mesh->m_VertexArray->Unbind();
	}

	void Renderer::SubmitQuad(const Ref<MaterialInstance>& material, const glm::mat4& transform)
	{
		bool depthTest = true;
		if (material)
		{
			material->Bind();
			auto shader = material->GetShader();
			shader->SetMat4("u_Transform", transform);
			depthTest = material->HasFlag(MaterialFlag::DepthTest);
		}
		s_Data->QuadVertexArray->Bind();
		RenderCommand::EnbaleDepthTest(depthTest);
		RenderCommand::DrawIndexed(6);
	}

	void Renderer::SubmitFullScreenQuad(const Ref<MaterialInstance>& material)
	{
		bool depthTest = true;
		if(material)
		{
			material->Bind();
			depthTest = material->HasFlag(MaterialFlag::DepthTest);
		}
		s_Data->QuadVertexArray->Bind();
		RenderCommand::EnbaleDepthTest(depthTest);
		RenderCommand::DrawIndexed(6);
	}
}
