#include "skpch.h"
#include "RenderCommandQueue.h"


namespace Spark
{
	constexpr int COMMAND_BUFFER_SIZE = 10 * 1024 * 1024; // 10 MB

	RenderCommandQueue::RenderCommandQueue()
	{
		m_CommandBuffer = new unsigned char[COMMAND_BUFFER_SIZE];
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer,0, COMMAND_BUFFER_SIZE);
	}

	RenderCommandQueue::~RenderCommandQueue()
	{
		delete[] m_CommandBuffer;
		m_CommandBuffer = nullptr;
		m_CommandBufferPtr = nullptr;
	}

	void* RenderCommandQueue::Allocate(RenderCommandFn func, uint32_t size)
	{
		*(RenderCommandFn*)m_CommandBufferPtr = func;
		m_CommandBufferPtr += sizeof(RenderCommandFn);
		*(int*)m_CommandBufferPtr = size;
		m_CommandBufferPtr += sizeof(uint32_t);
		void* memory = m_CommandBufferPtr;
		m_CommandBufferPtr += size;
		m_CommandCount++;
		return memory;
	}




	void RenderCommandQueue::Execute()
	{
		SK_CORE_TRACE("RenderCommandQueue::Execute --{0} commands,{1} bytes", m_CommandCount, (m_CommandBufferPtr - m_CommandBuffer));

		byte* buffer = m_CommandBuffer;
		for (uint32_t i = 0; i < m_CommandCount; ++i)
		{
			RenderCommandFn function = *(RenderCommandFn*)buffer;
			buffer += sizeof(RenderCommandFn);
			uint32_t size = *(uint32_t*)buffer;
			buffer += sizeof(uint32_t);
			function(buffer);
			buffer += size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}
}
