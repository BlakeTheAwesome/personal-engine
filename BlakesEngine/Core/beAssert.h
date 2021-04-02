#pragma once

import beTypes;

#include "beMacros.h"

#include "bePrintf.h"

#define ENABLE_ASSERTS 1

#define STATIC_ASSERT(x) static_assert(x, #x)
#ifdef ENABLE_ASSERTS
	#define BE_BREAKPOINT __debugbreak
	#define BE_ASSERT(x) do {if (!(x)){ LOG("ASSERT FAILED: " #x); BE_BREAKPOINT(); }} while (false)
	#define BE_ASSERT_CODE(x) x
#else
	#define BE_ASSERT(x) Unused(x)
	#define BE_ASSERT_CODE(x)
#endif

