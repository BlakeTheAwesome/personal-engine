#pragma once
#include "BlakesEngine\Core/beMacros.h"

#define bePRINTF(...) bePrintf::bePrintf(true, LOCATION, __VA_ARGS__)
//#define bePRINTF printf

namespace bePrintf
{

	void bePrintf(bool includeLocation, const char* location, const char* format, ...);
};

