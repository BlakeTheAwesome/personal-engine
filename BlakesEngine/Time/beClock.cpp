#include "bePCH.h"
#include "beClock.h"

#include <windows.h>

static double s_frequency = 0;
static __int64 s_startTime = 0;

void beClock::Initialise()
{
	LARGE_INTEGER li;
	if(!QueryPerformanceFrequency(&li))
	{
		__asm int 3;
	}
	__int64 ticksPerSecond = li.QuadPart;
	s_frequency = 1.0 / (double)(ticksPerSecond);

	QueryPerformanceCounter(&li);
	s_startTime = li.QuadPart;
}

double beClock::GetSecondsSinceStart()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart-s_startTime) * s_frequency;
}

double beClock::GetMillisecondsSinceStart()
{
	return GetSecondsSinceStart() * 1000;
}
