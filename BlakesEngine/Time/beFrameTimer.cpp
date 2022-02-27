module;
#include "BlakesEngine/bePCH.h"
#include <chrono>
module beFrameTimer;

using namespace std;

void beFrameTimer::LimitFPS(int maxFPS)
{
	Duration oneSecond = std::chrono::seconds(1);
	m_requiredWaitTime = oneSecond / maxFPS;
}

bool beFrameTimer::StepFrame(Duration* dt)
{
	TimePoint now = chrono::high_resolution_clock::now();
	Duration diff = now - m_lastTime;
	m_lastTime = now;

	m_accumulatedTime += diff;

	//using tFloatTime = std::chrono::duration<float>;
	//float secs = chrono::duration_cast<tFloatTime>(m_accumulatedTime).count();
	if (m_accumulatedTime > m_requiredWaitTime)
	{
		//LOG("elapsedTime:{} micros\r\n", secs);
		*dt = m_accumulatedTime;
		m_accumulatedTime = Duration::zero();
		return true;
	}
	//else
	//{
	//	LOG("elapsedTime only:{} micros\r\n", secs);
	//}
	return false;
}
