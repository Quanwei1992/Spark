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
	private:
		static RendererAPI* s_RendererAPI;
	};
}