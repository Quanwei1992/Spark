#include "skpch.h"

#include "Spark/Renderer/Buffer.h"
#include "Spark/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Spark
{
	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size,VertexBufferUsage usage)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
			case  RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(data, size,usage);
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case  RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size,usage);
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case  RendererAPI::API::None: SK_CORE_ASSERT(false, "RendererAPI::None is current not support!"); return nullptr;
		case  RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, size);
		}
		SK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}



	void BufferLayout::CalculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}
}
