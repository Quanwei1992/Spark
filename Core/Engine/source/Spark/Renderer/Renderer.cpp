#include "skpch.h"

#include "Spark/Renderer/Renderer.h"
#include "Spark/Renderer/RenderCommand.h"
#include "Spark/Renderer/RenderCommandQueue.h"
#include "Spark/Renderer/Shader.h"
#include "Spark/Renderer/Renderer2D.h"
namespace Spark
{

	struct RendererData
	{
		RenderCommandQueue CommandQueue;
	};

	static RendererData* s_Data = nullptr;


	void Renderer::Init()
	{
		SK_PROFILE_FUNCTION();
		RenderCommand::Init();
		Renderer2D::Init();

		s_Data = new RendererData;
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer::OnWindowResized(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0,0,width, height);
	}

	void Renderer::Clear(float r, float g, float b, float a)
	{
		float* params = (float*)s_Data->CommandQueue.Allocate(RenderCommand::Clear, sizeof(float) * 4);
		params[0] = r;
		params[1] = g;
		params[2] = b;
		params[3] = a;

	}

	void* Renderer::Submit(RenderCommandFn fn, uint32_t size)
	{
		return s_Data->CommandQueue.Allocate(fn, size);
	}

	void Renderer::WaitAndRender()
	{
		s_Data->CommandQueue.Execute();
	}
}
