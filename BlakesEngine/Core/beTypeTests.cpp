#include "bePCH.h"
#include "beTypeTests.h"
#include "blakesengine/Core/beTypes.h"
#include "blakesengine/Core/beAssert.h"

namespace beTypeTests
{
	bool RunTypeTests()
	{
		BE_ASSERT( sizeof(s8) == sizeof(u8) && (sizeof(s8) == 1 ));
		BE_ASSERT( sizeof(s16) == sizeof(u16) && (sizeof(s16) == 2 ));
		BE_ASSERT( sizeof(s32) == sizeof(u32) && (sizeof(s32) == 4 ));
		BE_ASSERT( sizeof(s64) == sizeof(u64) && (sizeof(s64) == 8 ));
		
		BE_ASSERT( false );
		return true;
	}
};
