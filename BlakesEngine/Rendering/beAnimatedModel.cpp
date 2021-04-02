#include "BlakesEngine/bePCH.h"
#include "beAnimatedModel.h"

import beDataBuffer;
import bePackedData;
import beByteStreams;

void Initialise(bePackedData const& packedData)
{
	#ifdef ENABLE_ASSERTS
	{
		beDataBuffer mainBlob = packedData.GetStream("main");
		beReadStream mainStream(mainBlob);
		u8 magic, version;
		mainStream >> magic >> version;
		BE_ASSERT(magic == 0xef);
		BE_ASSERT(version == 1);
	}
	#endif
	
	{
		//beDataBuffer formats = packedData.GetStream("formats");
	}
}

