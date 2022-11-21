#include "rzpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Razor
{
	static GLenum ShaderDateTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return  GL_FLOAT;
		case ShaderDataType::Float2:	return  GL_FLOAT;
		case ShaderDataType::Float3:	return  GL_FLOAT;
		case ShaderDataType::Float4:	return  GL_FLOAT;
		case ShaderDataType::Mat3:		return  GL_FLOAT;
		case ShaderDataType::Mat4:		return  GL_FLOAT;
		case ShaderDataType::Int:		return  GL_INT;
		case ShaderDataType::Int2:		return  GL_INT;
		case ShaderDataType::Int3:		return  GL_INT;
		case ShaderDataType::Int4:		return  GL_INT;
		case ShaderDataType::Bool:		return  GL_BOOL;
		}
		RZ_CORE_ASSERT(false, "Unkown ShaderDataType!");
		return 0;
	}


	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);

	}

	void OpenGLVertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) 
	{

		RZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		for (const auto& element : vertexBuffer->GetLayout())
		{

			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDateTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				vertexBuffer->GetLayout().GetStride(),
				(const void*)element.Offset);
			index++;
		}


		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) 
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
		m_IndexBuffers = indexBuffer;
	}

	const std::vector<std::shared_ptr<VertexBuffer>> OpenGLVertexArray::GetVertexBuffers() const
	{
		return m_VertexBuffers;
	}

	const std::shared_ptr<IndexBuffer> OpenGLVertexArray::GetIndexBuffer() const
	{
		return m_IndexBuffers;
	}



}

