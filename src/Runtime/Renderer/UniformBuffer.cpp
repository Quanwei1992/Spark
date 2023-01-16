#include "skpch.h"
#include "Spark/Renderer/UniformBuffer.h"
#include "Spark/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Spark
{
	namespace Spark
	{
	}
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size,binding);
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
