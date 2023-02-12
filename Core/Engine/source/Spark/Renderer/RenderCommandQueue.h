#pragma once

namespace Spark
{
	class RenderCommandQueue
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		RenderCommandQueue();
		~RenderCommandQueue();

		void* Allocate(RenderCommandFn func, uint32_t size);
		void Execute();

	private:
		unsigned char* m_CommandBuffer = nullptr;
		unsigned char* m_CommandBufferPtr = nullptr;
		unsigned int m_CommandCount = 0;
	};
}
