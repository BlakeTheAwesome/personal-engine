#include "bePCH.h"
#include "beFrameTimer.h"

beFrameTimer::beFrameTimer()
	: m_lastTime(beTimeValue::Now())
	, m_accumulatedTime(0)
	, m_requiredWaitTime(0)
{
}

void beFrameTimer::LimitFPS(int maxFPS)
{
	const double secondsToWait = 1.0 / (double)maxFPS;
	const double microsToWait = secondsToWait * 1000000.0;
	m_requiredWaitTime = beTimeValue((s64)microsToWait);
}

bool beFrameTimer::StepFrame(beTimeValue* dt)
{
	const beTimeValue now = beTimeValue::Now();
	beTimeValue diff = now - m_lastTime;
	m_lastTime = now;

	m_accumulatedTime += diff;

	if (m_accumulatedTime > m_requiredWaitTime)
	{
		*dt = m_accumulatedTime;
		m_accumulatedTime.Set(0);
		return true;
	}
	return false;
}
