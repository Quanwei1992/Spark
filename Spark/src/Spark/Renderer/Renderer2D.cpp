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
		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 100000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		Ref<VertexArray> VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = White texture


		glm::vec4 QuadVertexPositions[4];
		glm::vec2 QuadVertexTexCoords[4];

		Renderer2D::Statistics Stats;
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
				{ShaderDataType::Float2,"a_TexCoord"},
				{ShaderDataType::Float,"a_TexIndex"},
				{ShaderDataType::Float,"a_TilingFactor"}
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
		int32_t samplers[s_Data->MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data->MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}
		s_Data->TextureShader->SetIntArray("u_Textures", samplers, s_Data->MaxTextureSlots);


		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data->TextureSlots[0] = s_Data->WhiteTexture;

		s_Data->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data->QuadVertexTexCoords[0] = { 0.0f, 0.0f };
		s_Data->QuadVertexTexCoords[1] = { 1.0f, 0.0f };
		s_Data->QuadVertexTexCoords[2] = { 1.0f, 1.0f };
		s_Data->QuadVertexTexCoords[3] = { 0.0f, 1.0f };

	

	}
	void Renderer2D::Shutdown()
	{
		SK_PROFILE_FUNCTION();
		delete s_Data;
		s_Data = nullptr;
	}


	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
		s_Data->QuadIndexCount = 0;
		s_Data->TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		SK_PROFILE_FUNCTION();
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
		s_Data->QuadIndexCount = 0;
		s_Data->TextureSlotIndex = 1;
	}



	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size,rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		SK_PROFILE_FUNCTION();

		if (s_Data->QuadIndexCount  >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->TexCoord = s_Data->QuadVertexTexCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = 0.0f;
			s_Data->QuadVertexBufferPtr->TilingFactor = 1.0f;
			s_Data->QuadVertexBufferPtr++;
		}
		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
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

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = glm::vec4(1.0f);
			s_Data->QuadVertexBufferPtr->TexCoord = s_Data->QuadVertexTexCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = 0.0f;
			s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data->QuadVertexBufferPtr++;
		}
		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}


	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, texture, tilingFactor, tintColor);
	}



	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		SK_PROFILE_FUNCTION();

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->TexCoord = s_Data->QuadVertexTexCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = 0.0f;
			s_Data->QuadVertexBufferPtr->TilingFactor = 1.0f;
			s_Data->QuadVertexBufferPtr++;
		}
		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}


	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		SK_PROFILE_FUNCTION();

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		constexpr glm::vec4 color = { 1.0f,1.0f,1.0f,1.0f };

		int32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (*s_Data->TextureSlots[i].get() == *texture.get())
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			textureIndex = s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
			s_Data->TextureSlotIndex++;
		}


		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->TexCoord = s_Data->QuadVertexTexCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = (float)textureIndex;
			s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data->QuadVertexBufferPtr++;
		}
		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
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
		for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
		{
			s_Data->TextureSlots[i]->Bind(i);
		}
		RenderCommand::DrawIndexed(s_Data->VertexArray, s_Data->QuadIndexCount);

		s_Data->Stats.DrawCalls++;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->Stats;
	}

	void Renderer2D::ResetStats()
	{
		s_Data->Stats.DrawCalls = 0;
		s_Data->Stats.QuadCount = 0;
	}
}