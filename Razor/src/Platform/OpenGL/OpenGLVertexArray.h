#pragma once
#include "Razor/Renderer/VertexArray.h"


namespace Razor
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer)  override;
		virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)  override;
		virtual const std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const override;
		virtual const std::shared_ptr<IndexBuffer> GetIndexBuffer() const override;

	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffers;
		uint32_t m_RendererID;

	};
}