#pragma once
#include "RendererAPI.h"
#include "RenderCommandQueue.h"

namespace Spark
{
	class Shader;
	class Renderer
	{
	public:

		typedef void(*RenderCommandFn)(void*);

		static void Init();
		static void Shutdown();
		static void OnWindowResized(uint32_t width, uint32_t height);
		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		// Commands
		static void Clear(float r, float g, float b, float a);
		static void* Submit(RenderCommandFn fn, uint32_t size);
		static void WaitAndRender();
	};
}

#define SK_RENDER_PASTE2(a, b) a ## b
#define SK_RENDER_PASTE(a, b) SK_RENDER_PASTE2(a,b)
#define SK_RENDER_UNIQUE(x) SK_RENDER_PASTE(x, __LINE__)

#define SK_RENDER(code) \
	struct SK_RENDER_UNIQUE(SKRenderCommand) \
	{ \
		static void Execute(void*) \
		{ \
			code\
		} \
	}; \
	{\
		auto mem = ::Spark::Renderer::Submit(SK_RENDER_UNIQUE(SKRenderCommand)::Execute,sizeof(SK_RENDER_UNIQUE(SKRenderCommand)::Execute)));\
		new (mem)SK_RENDER_UNIQUE(SKRenderCommand)();\
	}\
