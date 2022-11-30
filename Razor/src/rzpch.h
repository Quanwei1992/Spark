#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <utility>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <cstdint>

#include "Razor/Core/Core.h"
#include "Razor/Core/Log.h"
#include "Debug/Instrumentor.h"


#ifdef RZ_PLATFORM_WINDOWS
	#include <Windows.h>
#endif