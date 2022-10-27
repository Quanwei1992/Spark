#pragma once

#ifdef RZ_PLATFORM_WINDOWS
	#ifdef RZ_BUILD_DLL
	#define RAZOR_API __declspec(dllexport)
	#else
	#define RAZOR_API __declspec(dllimport)
	#endif
#else
	#error Razor only supports Windows!
#endif

#define BIT(x) (1 << x)