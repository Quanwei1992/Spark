#include "skpch.h"

#include "OpenGLBuffer.h"

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
		:m_Size(size)
	{
		SK_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, data, Utils::toGLenum(usage));

	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage)
		:m_Size(size)
	{
		SK_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, Utils::toGLenum(usage));
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		SK_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_RendererID);
	}
	void OpenGLVertexBuffer::Bind() const
	{
		SK_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLVertexBuffer::Unbind() const
	{
		SK_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size,uint32_t offset)
	{
		m_Size = size;
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

	/////////////////////////////////////////////////////////////////////
	// Index Buffer /////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t size)
		:m_Size(size)
	{
		SK_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, indices, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		SK_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		m_Size = size;
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		SK_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLIndexBuffer::Unbind() const
	{
		SK_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	uint32_t OpenGLIndexBuffer::GetCount() const
	{
		return m_Size / sizeof(uint32_t);
	}
}
