#pragma once
#include "Spark/Renderer/VertexArray.h"


namespace Spark
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(Ref<VertexBuffer> vertexBuffer)  override;
		virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer)  override;
		virtual const std::vector<Ref<VertexBuffer>> GetVertexBuffers() const override;
		virtual const Ref<IndexBuffer> GetIndexBuffer() const override;

	private:
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffers;
		uint32_t m_RendererID;

	};
}