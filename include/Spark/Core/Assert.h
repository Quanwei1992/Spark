#pragma once

#include "Spark/Core/Base.h"
#include "Spark/Core/Log.h"
#include <filesystem>

#ifdef SK_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define SK_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { SK##type##ERROR(msg, __VA_ARGS__); SK_DEBUGBREAK(); } }
	#define SK_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SK_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define SK_INTERNAL_ASSERT_NO_MSG(type, check) SK_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", SK_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define SK_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define SK_INTERNAL_ASSERT_GET_MACRO(...) SK_EXPAND_MACRO( SK_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, SK_INTERNAL_ASSERT_WITH_MSG, SK_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define SK_ASSERT(...) SK_EXPAND_MACRO( SK_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define SK_CORE_ASSERT(...) SK_EXPAND_MACRO( SK_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define SK_ASSERT(...)
	#define SK_CORE_ASSERT(...)
#endif
