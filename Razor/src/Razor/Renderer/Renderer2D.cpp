#include "rzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

namespace Razor
{

	struct Renderer2DStorage
	{
		Ref<VertexArray> VertexArray;
		Ref<Shader> FlatColorShader;
	};

	static Renderer2DStorage* s_Data = nullptr;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();
		s_Data->VertexArray = VertexArray::Create();

		float squareVertices[5 * 3] = {
			-0.5f,-0.5f,0.0f,
			 0.5f,-0.5f,0.0f,
			 0.5f, 0.5f,0.0f,
			-0.5f, 0.5f,0.0f,
		};
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
				{ShaderDataType::Float3,"a_Position"}
			});

		s_Data->VertexArray->AddVertexBuffer(squareVB);
		uint32_t squareIndices[] = { 0,1,2,2,3,0 };
		Ref<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		s_Data->VertexArray->SetIndexBuffer(squareIB);

		s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColorShader.glsl");
	}
	void Renderer2D::Shutdown()
	{
		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->FlatColorShader->Bind();
		s_Data->FlatColorShader->UploadUniformMat4("u_ViewProjection",camera.GetViewProjectionMatrix());
		s_Data->FlatColorShader->UploadUniformMat4("u_Transform", glm::mat4(1.0f));
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->FlatColorShader->UploadUniformFloat4("u_Color", color);
		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}
}