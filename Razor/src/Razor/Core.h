#pragma once

#include <memory>

#ifdef RZ_ENABLE_ASSERTS
	#define RZ_ASSERT(x, ...) {if(!(x)){RZ_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
	#define RZ_CORE_ASSERT(x, ...) {if(!(x)){RZ_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
#else
	#define RZ_ASSERT(x, ...)
	#define RZ_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1 << x)

#define RZ_BIND_EVENT_FN(x) std::bind(&x,this,std::placeholders::_1)

namespace Razor
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}