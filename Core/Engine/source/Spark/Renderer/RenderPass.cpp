#include "skpch.h"
#include "RenderPass.h"
#include "Renderer.h"
#include "Platform/opengl/OpenGLRenderPass.h"
namespace Spark
{
	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLRenderPass>(spec);
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
