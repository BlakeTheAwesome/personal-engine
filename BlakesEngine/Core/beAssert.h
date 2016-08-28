#pragma once

#include "beTypes.h"
#include "beMacros.h"

#define ENABLE_ASSERTS 1

#define STATIC_ASSERT(x) static_assert(x, #x)
#ifdef ENABLE_ASSERTS
	#define BE_ASSERT(x) beAssert::DebugBreak((x), #x, 3, LOCATION )
	#define BE_ASSERT_CODE(x) x
#else
	#define BE_ASSERT(x) BE_UNUSED(x)
	#define BE_ASSERT_CODE(x)
#endif

namespace beAssert
{
	void DebugBreak( bool condition, const char* conditionAsString, s8 warningLevel, const char* location );
}

