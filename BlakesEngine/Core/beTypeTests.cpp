#include "BlakesEngine/bePCH.h"
#include "beTypeTests.h"
#include "blakesengine/Core/beTypes.h"
#include "blakesengine/Core/beAssert.h"

namespace beTypeTests
{
	bool RunTypeTests()
	{
		static_assert( sizeof(s8)  == sizeof(u8)  && (sizeof(s8)  == 1 ), "");
		static_assert( sizeof(s16) == sizeof(u16) && (sizeof(s16) == 2 ), "");
		static_assert( sizeof(s32) == sizeof(u32) && (sizeof(s32) == 4 ), "");
		static_assert( sizeof(s64) == sizeof(u64) && (sizeof(s64) == 8 ), "");
		
		BE_ASSERT( false );
		return true;
	}
};
