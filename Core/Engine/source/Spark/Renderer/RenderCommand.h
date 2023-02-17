#pragma once

#include "RendererAPI.h"

namespace Spark
{
	class RenderCommand
	{
	public:
		static inline void Init()
		{
			s_RendererAPI->Init();
		}

		static inline void Shutdown()
		{
			s_RendererAPI->Shutdown();
		}

		static inline void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x,y,width,height);
		}

		static inline void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		static inline void Clear()
		{
			s_RendererAPI->Clear();
		}

		static inline void DrawIndexed(const Ref<VertexArray>& vertexArray,uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		static inline void DrawIndexed(uint32_t count)
		{
			s_RendererAPI->DrawIndexed(count);
		}

		static inline void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		static inline void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}

		static inline void EnbaleDepthTest(bool enbale)
		{
			s_RendererAPI->EnableDepthTest(enbale);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}
