#pragma once

#include "Spark/Core/Buffer.h"
#include "Spark/Renderer/Buffer.h"

namespace Spark
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const void* data,uint32_t size,VertexBufferUsage usage);
		OpenGLVertexBuffer(uint32_t size,VertexBufferUsage usage);
		~OpenGLVertexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;

		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual const BufferLayout& GetLayout() const  override { return m_Layout; }

		uint32_t GetSize() const override {return m_Size;}

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Size = 0;
		BufferLayout m_Layout;
		Buffer m_LocalData;
		VertexBufferUsage m_Usage;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		~OpenGLIndexBuffer() override;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual uint32_t GetCount() const override;
		uint32_t GetSize() const override { return m_Size; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Size = 0;
		Buffer m_LocalData;
	};
}
