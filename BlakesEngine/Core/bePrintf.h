#pragma once
#include "Core/beMacros.h"

#define bePRINTF(...) bePrintf::bePrintf(LOCATION, __VA_ARGS__)
//#define bePRINTF printf

namespace bePrintf
{

	void bePrintf(const char* location, const char* format, ...);
};

