#include "BlakesEngine_PCH.h"

#include "Core/beTypeTests.h"

namespace beTypeTests
{
	bool RunTypeTests()
	{
		beASSERT( sizeof(s8) == sizeof(u8) && (sizeof(s8) == 1 ));
		beASSERT( sizeof(s16) == sizeof(u16) && (sizeof(s16) == 2 ));
		beASSERT( sizeof(s32) == sizeof(u32) && (sizeof(s32) == 4 ));
		beASSERT( sizeof(s64) == sizeof(u64) && (sizeof(s64) == 8 ));
		
		beASSERT( false );
		return true;
	}
};
