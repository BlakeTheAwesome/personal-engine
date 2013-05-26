#pragma once
#include "beTimeValue.h"

class beFrameTimer
{
	public:
		beFrameTimer();

		void LimitFPS(int maxFPS);
		bool StepFrame(beTimeValue* dt);

	private:
		beTimeValue m_lastTime;
		beTimeValue m_accumulatedTime;
		beTimeValue m_requiredWaitTime;
};
