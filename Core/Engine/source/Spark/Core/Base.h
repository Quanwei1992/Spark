#pragma once


#include <memory>
#include <glm.hpp>

#ifdef SK_DEBUG
#if defined(SK_PLATFORM_WINDOWS)
#define SK_DEBUGBREAK() __debugbreak()
#elif defined(SK_PLATFORM_LINUX)
#include <signal.h>
#define SK_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define SK_ENABLE_ASSERTS
#else
#define SK_DEBUGBREAK()
#endif

#define SK_EXPAND_MACRO(x) x
#define SK_STRINGIFY_MACRO(x) #x


#define BIT(x) (1 << x)

#define SK_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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

	using byte = uint8_t;
}


#include "Spark/Core/Log.h"
#include "Spark/Core/Assert.h"
