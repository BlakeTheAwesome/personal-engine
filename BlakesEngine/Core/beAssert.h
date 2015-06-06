#ifndef _beAssert_h_
#define _beAssert_h_

#include "blakesengine/Core/beTypes.h"
#include "blakesengine/Core/beMacros.h"

#define ENABLE_ASSERTS 1

#ifdef ENABLE_ASSERTS
	#define BE_ASSERT(x) beAssert::DebugBreak((x), #x, 3, LOCATION )
	#define BE_ASSERT_CODE(x) x
#else
	#define BE_ASSERT(x) BE_UNUSED(x)
	#define BE_ASSERT_CODE(x)
#endif

#define BA_ASSERT_COMPILETIME(x) {int i = 0; switch(i){case 0: case(x):;}}

namespace beAssert
{
	void DebugBreak( bool condition, const char* conditionAsString, s8 warningLevel, const char* location );
}

#endif
