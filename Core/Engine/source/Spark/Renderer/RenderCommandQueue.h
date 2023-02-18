#pragma once

namespace Spark
{
	typedef void(*RenderCommandFn)(void*);

	class RenderCommandQueue
	{
	public:
		static void Init();
		static void Shutdown();
		static void Execute();

		template <typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr)
			{
				auto pFunc = static_cast<FuncT*>(ptr);
				(*pFunc)();
				pFunc->~FuncT();
			};
			auto storageBuffer = Allocate(renderCmd, sizeof(func));
			new(storageBuffer)FuncT(std::forward<FuncT>(func));
		}
	private:
		static void* Allocate(RenderCommandFn func, uint32_t size);
	};
}
