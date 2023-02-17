#include "skpch.h"
#include "Spark/Renderer/Renderer2D.h"

#include "Spark/Renderer/VertexArray.h"
#include "Spark/Renderer/Shader.h"
#include "Spark/Renderer/RenderCommand.h"
#include "Spark/Renderer/UniformBuffer.h"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Spark
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		int TexIndex;
		float TilingFactor;

		// Editor-only
		int EntityID = -1;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID = -1;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		// Editor-only
		int EntityID = -1;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 100000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps


		// Quad Data

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = White texture

		glm::vec4 QuadVertexPositions[4];
		glm::vec2 QuadVertexTexCoords[4];

		Ref<Texture2D> WhiteTexture;

		// Circle Data

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;


		// Line Data

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData* s_Data = nullptr;

	void Renderer2D::Init()
	{
		SK_PROFILE_FUNCTION();
		s_Data = new Renderer2DData();

		// Uniform buffer

		s_Data->CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		// Quads
		s_Data->QuadVertexArray = VertexArray::Create();

		s_Data->QuadVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));

		s_Data->QuadVertexBuffer->SetLayout({
				{ShaderDataType::Float3,"a_Position"},
				{ShaderDataType::Float4,"a_Color"},
				{ShaderDataType::Float2,"a_TexCoord"},
				{ShaderDataType::Int,"a_TexIndex"},
				{ShaderDataType::Float,"a_TilingFactor"},
				{ShaderDataType::Int,"a_EntityID"}
			});

		s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);
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


		Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, s_Data->MaxIndices * sizeof(uint32_t));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		delete[] quadIndices;

		s_Data->WhiteTexture = Texture2D::Create(TextureFormat::RGBA,1, 1);
		s_Data->WhiteTexture->Lock();
		s_Data->WhiteTexture->GetWrieableBuffer()[0] = 0xffffffff;
		s_Data->WhiteTexture->Unlock();

		s_Data->TextureSlots[0] = s_Data->WhiteTexture;

		s_Data->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data->QuadVertexTexCoords[0] = { 0.0f, 0.0f };
		s_Data->QuadVertexTexCoords[1] = { 1.0f, 0.0f };
		s_Data->QuadVertexTexCoords[2] = { 1.0f, 1.0f };
		s_Data->QuadVertexTexCoords[3] = { 0.0f, 1.0f };

		s_Data->QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
		s_Data->QuadShader->Bind();
		int32_t samplers[s_Data->MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data->MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		// Circles

		s_Data->CircleVertexArray = VertexArray::Create();

		s_Data->CircleVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(CircleVertex));

		s_Data->CircleVertexBuffer->SetLayout({
				{ShaderDataType::Float3,"a_WorldPosition"},
				{ShaderDataType::Float3,"a_LocalPosition"},
				{ShaderDataType::Float4,"a_Color"},
				{ShaderDataType::Float,"a_Thickness"},
				{ShaderDataType::Float,"a_Fade"},
				{ShaderDataType::Int,"a_EntityID"}
			});

		s_Data->CircleVertexArray->AddVertexBuffer(s_Data->CircleVertexBuffer);
		s_Data->CircleVertexBufferBase = new CircleVertex[s_Data->MaxVertices];
		s_Data->CircleVertexArray->SetIndexBuffer(squareIB); // Use quad index buffer

		s_Data->CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");


		// Liens
		s_Data->LineVertexArray = VertexArray::Create();

		s_Data->LineVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(LineVertex));

		s_Data->LineVertexBuffer->SetLayout({
				{ShaderDataType::Float3,"a_Position"},
				{ShaderDataType::Float4,"a_Color"},
				{ShaderDataType::Int,"a_EntityID"}
			});

		s_Data->LineVertexArray->AddVertexBuffer(s_Data->LineVertexBuffer);
		s_Data->LineVertexBufferBase = new LineVertex[s_Data->MaxVertices];

		s_Data->LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");

	}
	void Renderer2D::Shutdown()
	{
		SK_PROFILE_FUNCTION();
		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer2D::BeginScene(const glm::mat4& viewProjection)
	{

		s_Data->CameraBuffer.ViewProjection = viewProjection;
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(Renderer2DData::CameraBuffer));

		StartBatch();
	}


	void Renderer2D::StartBatch()
	{
		s_Data->QuadIndexCount = 0;
		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;

		s_Data->CircleIndexCount = 0;
		s_Data->CircleVertexBufferPtr = s_Data->CircleVertexBufferBase;

		s_Data->LineVertexCount = 0;
		s_Data->LineVertexBufferPtr = s_Data->LineVertexBufferBase;

		s_Data->TextureSlotIndex = 1;
	}

	inline void Renderer2D::DrawQuadImpl(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			Flush();
			StartBatch();
		}
		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->TexCoord = s_Data->QuadVertexTexCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = 0;
			s_Data->QuadVertexBufferPtr->TilingFactor = 1.0f;
			s_Data->QuadVertexBufferPtr->EntityID = entityID;
			s_Data->QuadVertexBufferPtr++;
		}
		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}

	inline void Renderer2D::DrawQuadImpl(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2* texCoords, float tilingFactor, const glm::vec4& tintColor,int entityID)
	{

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			Flush();
			StartBatch();
		}

		int32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (*s_Data->TextureSlots[i].get() == *(texture).get())
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
			s_Data->QuadVertexBufferPtr->Color = tintColor;
			s_Data->QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data->QuadVertexBufferPtr->EntityID = entityID;
			s_Data->QuadVertexBufferPtr++;
		}
		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}




	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size,rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		SK_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuadImpl(transform, color);
	}


	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size,rotation, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		SK_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuadImpl(transform, texture,s_Data->QuadVertexTexCoords, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		SK_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuadImpl(transform, texture->GetTexture(), texture->GetTexCoords(), tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		DrawQuadImpl(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuadImpl(transform, texture, s_Data->QuadVertexTexCoords, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform,const Ref<SubTexture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuadImpl(transform, texture->GetTexture(), texture->GetTexCoords(), tilingFactor, tintColor);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
	{
		// TODO: Implement for circles
		//if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		//{
		//	FlushAndReset();
		//}
		for (size_t i = 0; i < 4; i++)
		{
			s_Data->CircleVertexBufferPtr->WorldPosition = transform * s_Data->QuadVertexPositions[i];
			s_Data->CircleVertexBufferPtr->LocalPosition = s_Data->QuadVertexPositions[i] * 2.0f;
			s_Data->CircleVertexBufferPtr->Color = color;
			s_Data->CircleVertexBufferPtr->Thickness = thickness;
			s_Data->CircleVertexBufferPtr->Fade = fade;
			s_Data->CircleVertexBufferPtr->EntityID = entityID;
			s_Data->CircleVertexBufferPtr++;
		}
		s_Data->CircleIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, CircleRendererComponent& src, int entityID)
	{
		DrawCircle(transform, src.Color, src.Thickness, src.Fade, entityID);
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data->LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data->LineWidth = width;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		s_Data->LineVertexBufferPtr->Position = p0;
		s_Data->LineVertexBufferPtr->Color = color;
		s_Data->LineVertexBufferPtr->EntityID = entityID;

		s_Data->LineVertexBufferPtr++;

		s_Data->LineVertexBufferPtr->Position = p1;
		s_Data->LineVertexBufferPtr->Color = color;
		s_Data->LineVertexBufferPtr->EntityID = entityID;

		s_Data->LineVertexBufferPtr++;

		s_Data->LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID /*= -1*/)
	{

		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1,color,entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID /*= -1*/)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0;i<4;i++)
		{
			lineVertices[i] = transform * s_Data->QuadVertexPositions[i];
		}
		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}
	

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
		{
			DrawQuadImpl(transform, src.Texture, s_Data->QuadVertexTexCoords, src.TilingFactor, src.Color, entityID);
		}
		else {
			DrawQuadImpl(transform, src.Color, entityID);
		}	
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

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		SK_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuadImpl(transform, color);
	}


	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		SK_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuadImpl(transform, texture, s_Data->QuadVertexTexCoords, tilingFactor, tintColor);
	}


	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& texture
		, float tilingFactor, const glm::vec4& tintColor)
	{
		SK_PROFILE_FUNCTION();


		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuadImpl(transform, texture->GetTexture(), texture->GetTexCoords(), tilingFactor, tintColor);
	}

	void Renderer2D::EndScene()
	{
		SK_PROFILE_FUNCTION();

		Flush();
	}
	void Renderer2D::Flush()
	{

		if (s_Data->QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
			s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);


			for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
			{
				s_Data->TextureSlots[i]->Bind(i);
			}
			s_Data->QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Data->QuadVertexArray, s_Data->QuadIndexCount);

			s_Data->Stats.DrawCalls++;
		}

		if (s_Data->CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->CircleVertexBufferPtr - (uint8_t*)s_Data->CircleVertexBufferBase);
			s_Data->CircleVertexBuffer->SetData(s_Data->CircleVertexBufferBase, dataSize);

			s_Data->CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Data->CircleVertexArray, s_Data->CircleIndexCount);

			s_Data->Stats.DrawCalls++;
		}

		if (s_Data->LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->LineVertexBufferPtr - (uint8_t*)s_Data->LineVertexBufferBase);
			s_Data->LineVertexBuffer->SetData(s_Data->LineVertexBufferBase, dataSize);

			s_Data->LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data->LineWidth);
			RenderCommand::DrawLines(s_Data->LineVertexArray, s_Data->LineVertexCount);

			s_Data->Stats.DrawCalls++;
		}
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
