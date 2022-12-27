#pragma once

#include "Spark/Renderer/UniformBuffer.h"

namespace Spark
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;
	private:
		uint32_t m_RendererID = 0;
	};
}
