#include "BlakesEngine_PCH.h"
#include "beAssert.h"
#include "blakesengine/core/beMacros.h"
#include "blakesengine/core/bePrintf.h"
#include <stdio.h>
namespace beAssert
{
	void DebugBreak( bool condition, const char* conditionAsString, s8 warningLevel, const char* location )
	{
		BE_UNUSED( warningLevel );
		
		if ( !condition )
		{
			bePRINTF( location );
			bePRINTF( conditionAsString );
			__asm int 3;
		}
	}
}
