#pragma once
#include "BlakesEngine/Core/beMacros.h"
#include"BlakesEngine/External/fmt/format.h"

#define LOG(...) fmt::print(LOCATION ": " __VA_ARGS__)
//#define LOG(...) bePrintf::bePrintf(true, LOCATION, __VA_ARGS__)
//#define LOG printf

namespace bePrintf
{
	template <typename... Args>
	void bePrintf(Args&& ...args) { fmt::print(std::forward<Args>(args)...); }
	//void bePrintf(bool includeLocation, const char* location, const char* format, ...);
};

