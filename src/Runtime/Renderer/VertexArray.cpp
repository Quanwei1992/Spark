#include "skpch.h"
#include "Spark/Renderer/VertexArray.h"
#include "Spark/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Spark
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
