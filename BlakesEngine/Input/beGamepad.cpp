#include "bePCH.h"
#include "beGamepad.h"

#include "BlakesEngine/Core/beAssert.h"

//#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <xinput.h>

#pragma comment(lib, "xinput.lib")

bool beGamepad::Init(char index)
{
	m_index = index;
	return true;
}

void beGamepad::Deinit()
{
}

void beGamepad::Update(float dt)
{
	XINPUT_STATE state = {0};
	DWORD res = XInputGetState(m_index, &state);

	m_lastStateFlags = m_thisStateFlags;

	if(res == ERROR_SUCCESS)
	{
		m_connected = true;
		if (m_lastPacketNumber != state.dwPacketNumber)
		{
			m_lastPacketNumber = state.dwPacketNumber;
			m_thisStateFlags = state.Gamepad.wButtons;
			
			BYTE leftTrigger = state.Gamepad.bLeftTrigger;
			BYTE rightTrigger = state.Gamepad.bRightTrigger;
			SHORT leftX = state.Gamepad.sThumbLX;
			SHORT leftY = state.Gamepad.sThumbLY;
			SHORT rightX = state.Gamepad.sThumbRX;
			SHORT rightY = state.Gamepad.sThumbRY;

			if (leftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				m_l2 = 0.0f;
			}
			else
			{
				m_l2 = ((float)leftTrigger) / 255.0f;
			}
			
			if (rightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				m_r2 = 0.0f;
			}
			else
			{
				m_r2 = ((float)rightTrigger) / 255.0f;
			}

			if (leftX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && leftX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				m_leftX = 0.0f;
			}
			else
			{
				m_leftX = ((float)leftX) / 32768.0f;
			}

			if (leftY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && leftY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				m_leftY = 0.0f;
			}
			else
			{
				m_leftY = ((float)leftY) / 32768.0f;
			}

			if (rightX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && rightX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			{
				m_rightX = 0.0f;
			}
			else
			{
				m_rightX = ((float)rightX) / 32768.0f;
			}

			if (rightY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && rightY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			{
				m_rightY = 0.0f;
			}
			else
			{
				m_rightY = ((float)rightY) / 32768.0f;
			}
		}
	}
	else
	{
		m_connected = false;
		m_thisStateFlags = 0;
	}
}


float beGamepad::GetLeftX() const
{
	return m_leftX;
}

float beGamepad::GetLeftY() const
{
	return m_leftY;
}

float beGamepad::GetRightX() const
{
	return m_rightX;
}

float beGamepad::GetRightY() const
{
	return m_rightY;
}

float beGamepad::GetL2() const
{
	return m_l2;
}

float beGamepad::GetR2() const
{
	return m_r2;
}

bool beGamepad::GetPressed(Button button) const
{
	switch(button)
	{
		case A: return (m_thisStateFlags & XINPUT_GAMEPAD_A) != 0;
		case B: return (m_thisStateFlags & XINPUT_GAMEPAD_B) != 0;
		case X: return (m_thisStateFlags & XINPUT_GAMEPAD_X) != 0;
		case Y: return (m_thisStateFlags & XINPUT_GAMEPAD_Y) != 0;
		case L1: return (m_thisStateFlags & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
		case R1: return (m_thisStateFlags & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
		case Select: return (m_thisStateFlags & XINPUT_GAMEPAD_BACK) != 0;
		case Start: return (m_thisStateFlags & XINPUT_GAMEPAD_START) != 0;
		case Up: return (m_thisStateFlags & XINPUT_GAMEPAD_DPAD_UP) != 0;
		case Down: return (m_thisStateFlags & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		case Left: return (m_thisStateFlags & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		case Right: return (m_thisStateFlags & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
	}

	BE_ASSERT(false);
	return false;
}

bool beGamepad::GetButtonReleased(Button button) const
{
	switch(button)
	{
		case A: return (m_lastStateFlags & XINPUT_GAMEPAD_A) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_A) == 0);
		case B: return (m_lastStateFlags & XINPUT_GAMEPAD_B) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_B) == 0);
		case X: return (m_lastStateFlags & XINPUT_GAMEPAD_X) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_X) == 0);
		case Y: return (m_lastStateFlags & XINPUT_GAMEPAD_Y) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_Y) == 0);
		case L1: return (m_lastStateFlags & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_LEFT_SHOULDER) == 0);
		case R1: return (m_lastStateFlags & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_RIGHT_SHOULDER) == 0);
		case Select: return (m_lastStateFlags & XINPUT_GAMEPAD_BACK) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_BACK) == 0);
		case Start: return (m_lastStateFlags & XINPUT_GAMEPAD_START) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_START) == 0);
		case Up: return (m_lastStateFlags & XINPUT_GAMEPAD_DPAD_UP) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_DPAD_UP) == 0);
		case Down: return (m_lastStateFlags & XINPUT_GAMEPAD_DPAD_DOWN) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_DPAD_DOWN) == 0);
		case Left: return (m_lastStateFlags & XINPUT_GAMEPAD_DPAD_LEFT) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_DPAD_LEFT) == 0);
		case Right: return (m_lastStateFlags & XINPUT_GAMEPAD_DPAD_RIGHT) != 0 && ((m_thisStateFlags & XINPUT_GAMEPAD_DPAD_RIGHT) == 0);
	}

	BE_ASSERT(false);
	return false;
}
