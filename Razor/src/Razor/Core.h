#pragma once


#ifdef RZ_ENABLE_ASSERTS
	#define RZ_ASSERT(x, ...) {if(!(x)){RZ_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
	#define RZ_CORE_ASSERT(x, ...) {if(!(x)){RZ_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
#else
	#define RZ_ASSERT(x, ...)
	#define RZ_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1 << x)