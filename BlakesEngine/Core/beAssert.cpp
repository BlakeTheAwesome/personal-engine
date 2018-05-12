#include "BlakesEngine/bePCH.h"
#include "beAssert.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/bePrintf.h"
#include <stdio.h>
namespace beAssert
{
	void DebugBreak( bool condition, const char* conditionAsString, s8 warningLevel, const char* location )
	{
		STATIC_ASSERT(sizeof(char)==1);
		BE_UNUSED( warningLevel );
		
		if ( !condition )
		{
			bePrintf::bePrintf(location, conditionAsString);
			__debugbreak();
		}
	}
}
