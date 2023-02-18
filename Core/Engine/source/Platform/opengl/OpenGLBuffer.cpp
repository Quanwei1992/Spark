#include "skpch.h"
#include "OpenGLBuffer.h"
#include "Spark/Renderer/RenderCommandQueue.h"
#include <glad/glad.h>

namespace Spark
{

	namespace Utils
	{
		GLenum toGLenum(VertexBufferUsage usage)
		{
			switch (usage)
			{
			case VertexBufferUsage::Static: return GL_STATIC_DRAW;
			case VertexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
			}
			SK_CORE_ASSERT(false, "Unkown vertex buffer usage.");
			return 0;
		}
	}


	/////////////////////////////////////////////////////////////////////
	// Vertex Buffer /////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint32_t size, VertexBufferUsage usage)
		:m_Size(size),m_Usage(usage)
	{
		m_LocalData = Buffer::Copy(data, size);

		RenderCommandQueue::Submit([this]()
		{
			glCreateBuffers(1, &m_RendererID);
			glNamedBufferData(m_RendererID, m_Size, m_LocalData.Data, Utils::toGLenum(m_Usage));
		});
	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage)
		:m_Size(size), m_Usage(usage)
	{
		RenderCommandQueue::Submit([this]()
		{
			glCreateBuffers(1, &m_RendererID);
			glNamedBufferData(m_RendererID, m_Size, nullptr, Utils::toGLenum(m_Usage));
		});
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		RenderCommandQueue::Submit([this]()
		{
			glDeleteBuffers(1, &m_RendererID);
		});
	}
	void OpenGLVertexBuffer::Bind() const
	{
		RenderCommandQueue::Submit([this]()
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		});
	}
	void OpenGLVertexBuffer::Unbind() const
	{
		RenderCommandQueue::Submit([]()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		});
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size,uint32_t offset)
	{
		m_Size = size;
		m_LocalData = Buffer::Copy(data, size);
		RenderCommandQueue::Submit([this, offset]()
		{
			glNamedBufferSubData(m_RendererID, offset, m_Size, m_LocalData.Data);
		});
	}

	/////////////////////////////////////////////////////////////////////
	// Index Buffer /////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t size)
		:m_Size(size)
	{
		m_LocalData = Buffer::Copy(indices, size);
		RenderCommandQueue::Submit([this]()
		{
			glCreateBuffers(1, &m_RendererID);
			glNamedBufferData(m_RendererID, m_Size, m_LocalData.Data, GL_STATIC_DRAW);
		});
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		RenderCommandQueue::Submit([this]()
		{
			glDeleteBuffers(1, &m_RendererID);
		});
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		m_Size = size;
		m_LocalData = Buffer::Copy(data, size);
		RenderCommandQueue::Submit([this,offset]()
		{
			glNamedBufferSubData(m_RendererID, offset, m_Size, m_LocalData.Data);
		});
	}

	void OpenGLIndexBuffer::Bind() const
	{
		RenderCommandQueue::Submit([this]()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		});
	}
	void OpenGLIndexBuffer::Unbind() const
	{
		RenderCommandQueue::Submit([]()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		});
	}
	uint32_t OpenGLIndexBuffer::GetCount() const
	{
		return m_Size / sizeof(uint32_t);
	}
}
