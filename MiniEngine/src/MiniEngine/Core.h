#pragma once

#ifdef ME_PLATFORM_WINDOWS
	#ifdef ME_BUILD_DLL
	#define MINI_API __declspec(dllexport)
	#else
	#define MINI_API __declspec(dllimport)
	#endif
#else
	#error MiniEngine only support Windows!
#endif