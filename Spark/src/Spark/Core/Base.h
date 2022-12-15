#pragma once

#include "Log.h"

#include <memory>
#include <glm/glm.hpp>

#ifdef SK_ENABLE_ASSERTS
	#define SK_ASSERT(x, ...) {if(!(x)){SK_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
	#define SK_CORE_ASSERT(x, ...) {if(!(x)){SK_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
#else
	#define SK_ASSERT(x, ...)
	#define SK_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1 << x)

#define SK_BIND_EVENT_FN(x) std::bind(&x,this,std::placeholders::_1)

namespace Spark
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	// Base Types
	typedef glm::vec2 Float2;
	typedef glm::vec3 Float3;
	typedef glm::vec4 Float4;

	typedef glm::vec4 Color4f;
	typedef glm::vec3 Color3f;
}