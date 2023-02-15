#include "skpch.h"

#include "Spark/Renderer/Renderer.h"
#include "Spark/Renderer/RenderCommand.h"
#include "Spark/Renderer/Shader.h"
#include "Spark/Renderer/Renderer2D.h"
#include "Spark/Renderer/Framebuffer.h"
namespace Spark
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();

	void Renderer::Init()
	{
		SK_PROFILE_FUNCTION();
		RenderCommand::Init();
		FramebufferPool::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		FramebufferPool::Shutdown();
		RenderCommand::Shutdown();
		delete m_SceneData;
		m_SceneData = nullptr;
	}

	void Renderer::OnWindowResized(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0,0,width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene()
	{

	}
	void Renderer::Submit(const Ref<Shader>& shader,
		const Ref<VertexArray>& vertexArray,
		const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
