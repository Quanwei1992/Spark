#include "skpch.h"
#include "RenderCommandQueue.h"

namespace Spark
{

	struct RenderCommandQueueData
	{
		uint8_t* CommandBuffer = nullptr;
		uint8_t* CommandBufferPtr = nullptr;
		uint32_t CommandCount = 0;
	};

	static RenderCommandQueueData* s_Data = nullptr;

	void RenderCommandQueue::Init()
	{
		s_Data = new RenderCommandQueueData();
		s_Data->CommandBuffer = new uint8_t[10 * 1024 * 1024]; // 10 MB buffer
		s_Data->CommandBufferPtr = s_Data->CommandBuffer;
	}

	void RenderCommandQueue::Shutdown()
	{
		delete[] s_Data->CommandBuffer;
		delete s_Data;
		s_Data = nullptr;
	}

	void* RenderCommandQueue::Allocate(RenderCommandFn fn, uint32_t size)
	{
		*(RenderCommandFn*)s_Data->CommandBufferPtr = fn;
		s_Data->CommandBufferPtr += sizeof(RenderCommandFn);

		*(uint32_t*)s_Data->CommandBufferPtr = size;
		s_Data->CommandBufferPtr += sizeof(uint32_t);

		void* memory = s_Data->CommandBufferPtr;
		s_Data->CommandBufferPtr += size;

		s_Data->CommandCount++;
		return memory;
	}

	void RenderCommandQueue::Execute()
	{
		byte* buffer = s_Data->CommandBuffer;

		for (uint32_t i = 0; i < s_Data->CommandCount; i++)
		{
			RenderCommandFn function = *(RenderCommandFn*)buffer;
			buffer += sizeof(RenderCommandFn);

			uint32_t size = *(uint32_t*)buffer;
			buffer += sizeof(uint32_t);
			function(buffer);
			buffer += size;
		}

		s_Data->CommandBufferPtr = s_Data->CommandBuffer;
		s_Data->CommandCount = 0;
	}
}
