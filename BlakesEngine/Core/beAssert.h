#ifndef _beAssert_h_
#define _beAssert_h_

#include "BlakesEngine_PCH.h"

#include "Core/beTypes.h"
#include "Core/beMacros.h"

#ifdef _DEBUG
	#define beASSERT(x) beAssert::DebugBreak((x), #x, 3, beLOCATION )
#else
	#define beASSERT(x) beUNUSED(x)
#endif

namespace beAssert
{
	void DebugBreak( bool condition, const char* conditionAsString, s8 warningLevel, const char* location );
}

#endif
