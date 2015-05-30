#include "bePCH.h"
#include "beAssert.h"
#include "blakesengine/core/beMacros.h"
#include "blakesengine/core/bePrintf.h"
#include <stdio.h>
namespace beAssert
{
	void DebugBreak( bool condition, const char* conditionAsString, s8 warningLevel, const char* location )
	{
		BA_ASSERT_COMPILETIME(sizeof(char)==1);
		BE_UNUSED( warningLevel );
		
		if ( !condition )
		{
			bePrintf::bePrintf(true, location, conditionAsString);
			__asm int 3;
		}
	}
}
