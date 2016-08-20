#include "BlakesEngine/bePCH.h"
#include "beRandom.h"
#include <ctime>

void beRandom::InitFromSystemTime()
{
	Init((int)time(nullptr));
}
