#include "rzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Spark
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
	};

	struct Renderer2DData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		Ref<VertexArray> VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

	};

	static Renderer2DData* s_Data = nullptr;



	void Renderer2D::Init()
	{
		SK_PROFILE_FUNCTION();
		s_Data = new Renderer2DData();
		s_Data->VertexArray = VertexArray::Create();

		s_Data->VertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));

		s_Data->VertexBuffer->SetLayout({
				{ShaderDataType::Float3,"a_Position"},
				{ShaderDataType::Float4,"a_Color"},
				{ShaderDataType::Float2,"a_TexCoord"}
			});

		s_Data->VertexArray->AddVertexBuffer(s_Data->VertexBuffer);
		s_Data->QuadVertexBufferBase = new QuadVertex[s_Data->MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data->MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
			offset += 4;
		}


		Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, s_Data->MaxIndices);
		s_Data->VertexArray->SetIndexBuffer(squareIB);

		delete[] quadIndices;

		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);


		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	}
	void Renderer2D::Shutdown()
	{
		SK_PROFILE_FUNCTION();
		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		SK_PROFILE_FUNCTION();
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_Data->WhiteTexture->Bind();

		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
		s_Data->QuadIndexCount = 0;
	}



	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size,rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		SK_PROFILE_FUNCTION();
		s_Data->TextureShader->SetFloat4("u_Color", color);
		s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_Data->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0f),rotation,glm::vec3(0.0f,0.0f,1.0f))
			* glm::scale(glm::mat4(1.0f), {size.x,size.y,1.0f});

		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size,rotation, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		SK_PROFILE_FUNCTION();
		s_Data->TextureShader->SetFloat4("u_Color", tintColor);
		s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);
		texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		SK_PROFILE_FUNCTION();


		s_Data->QuadVertexBufferPtr->Position = position;
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = {0,0};
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x + size.x,position.y,position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 0,0 };
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x + size.x,position.y + size.y,position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 0,0 };
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadVertexBufferPtr->Position = { position.x,position.y + size.y,position.z };
		s_Data->QuadVertexBufferPtr->Color = color;
		s_Data->QuadVertexBufferPtr->TexCoord = { 0,0 };
		s_Data->QuadVertexBufferPtr++;

		s_Data->QuadIndexCount+=6;

		//s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
		//s_Data->WhiteTexture->Bind();

		//glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		//	* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		//s_Data->TextureShader->SetMat4("u_Transform", transform);
		//s_Data->VertexArray->Bind();
		//RenderCommand::DrawIndexed(s_Data->VertexArray);


	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, texture,tilingFactor,tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		SK_PROFILE_FUNCTION();
		s_Data->TextureShader->SetFloat4("u_Color", tintColor);
		s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);
		texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::EndScene()
	{
		SK_PROFILE_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
		s_Data->VertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);

		Flush();
	}
	void Renderer2D::Flush()
	{
		RenderCommand::DrawIndexed(s_Data->VertexArray, s_Data->QuadIndexCount);
	}
}