#include "BlakesEngine/bePCH.h"
#include "beClock.h"
#include "BlakesEngine/Core/beAssert.h"
#include <chrono>

using ClockType = std::chrono::high_resolution_clock;
using TimePoint = ClockType::time_point;
using Duration = ClockType::duration;
using DoubleSeconds = std::chrono::duration<double>;
using DoubleMillis = std::chrono::duration<double, std::milli>;

static TimePoint s_startTime;

void beClock::Initialise()
{
	s_startTime = ClockType::now();
}

double beClock::GetSecondsSinceStart()
{
	TimePoint now = ClockType::now();
	return std::chrono::duration_cast<DoubleSeconds>(now - s_startTime).count();
}

double beClock::GetMillisecondsSinceStart()
{
	TimePoint now = ClockType::now();
	return std::chrono::duration_cast<DoubleMillis>(now - s_startTime).count();
}

