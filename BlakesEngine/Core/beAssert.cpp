#include "BlakesEngine_PCH.h"
#include "beAssert.h"
#include "bePrintf.h"

namespace beAssert
{
	void DebugBreak( bool condition, const char* conditionAsString, s8 warningLevel, const char* location )
	{
		beUNUSED( warningLevel );
		
		if ( !condition )
		{
			bePRINTF( location );
			bePRINTF( conditionAsString );
			__asm int 3;
		}
	}
}
