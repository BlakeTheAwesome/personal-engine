#pragma once
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/External/fmt/format.h"

// #define LOG(msg, ...) fmt::print(LOCATION ": " msg "\r\n", __VA_ARGS__)
//#define LOG(...) bePrintf::bePrintf(true, LOCATION, __VA_ARGS__)
//#define LOG printf

namespace bePrintf
{
	template <typename... Args>
	void bePrintf(fmt::format_string<Args...> fmt, Args&&... args) { fmt::print(fmt, std::forward<Args>(args)...); }
	//void bePrintf(bool includeLocation, const char* location, const char* format, ...);
};

#define LOG bePrintf::bePrintf
