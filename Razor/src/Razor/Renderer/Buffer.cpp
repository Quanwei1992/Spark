#include "rzpch.h"

#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Razor
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: RZ_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
			case RendererAPI::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}
		RZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: RZ_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case RendererAPI::OpenGL: return new OpenGLIndexBuffer(indices, count);
		}
		RZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}