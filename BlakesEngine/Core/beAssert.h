#ifndef _beAssert_h_
#define _beAssert_h_

#include "blakesengine/Core/beTypes.h"
#include "blakesengine/Core/beMacros.h"

#ifdef _DEBUG
	#define BE_ASSERT(x) beAssert::DebugBreak((x), #x, 3, LOCATION )
#else
	#define BE_ASSERT(x) BE_UNUSED(x)
#endif

namespace beAssert
{
	void DebugBreak( bool condition, const char* conditionAsString, s8 warningLevel, const char* location );
}

#endif
