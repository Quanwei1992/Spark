#include "skpch.h"

#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Spark
{

	/////////////////////////////////////////////////////////////////////
	// Vertex Buffer /////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint32_t size)
	{
		SK_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		SK_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
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

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		SK_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	/////////////////////////////////////////////////////////////////////
	// Index Buffer /////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		:m_Count(count)
	{
		SK_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		SK_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_RendererID);
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
		return m_Count;
	}
}
