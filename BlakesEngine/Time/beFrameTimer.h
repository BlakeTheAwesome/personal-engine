#pragma once
#include "beTimeValue.h"

class beFrameTimer
{
public:
	using ClockType = std::chrono::high_resolution_clock;
	using TimePoint = ClockType::time_point;
	using Duration = ClockType::duration;

	void LimitFPS(int maxFPS);
	bool StepFrame(Duration* dt);

private:
	TimePoint m_lastTime = std::chrono::high_resolution_clock::now();
	Duration m_accumulatedTime = Duration::zero();
	Duration m_requiredWaitTime = Duration::zero();
};
