#include "BlakesEngine/bePCH.h"
#include "beMouse.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Platform/beSystemEventManager.h"
#include "BlakesEngine/Window/beWindow.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


beMouse::~beMouse()
{
	BE_ASSERT(!m_systemEventManager); 
}

void beMouse::Init(beSystemEventManager* systemEventManager, const beWindow* window)
{
	m_windowHeight = window->GetHeight();
	m_windowWidth = window->GetWidth();
	
	auto hinstance = (HINSTANCE*)window->GetHInstance();
	auto hWnd = *(HWND*)window->GetHWnd();
	auto result = DirectInput8Create(*hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, nullptr);
	BE_ASSERT(result >= 0);
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, nullptr);
	BE_ASSERT(result >= 0);
	result = m_mouse->SetDataFormat(&c_dfDIMouse2);
	BE_ASSERT(result >= 0);
	result = m_mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	BE_ASSERT(result >= 0);

	m_mouse->Acquire();
}

void beMouse::Deinit()
{
	m_mouse->Unacquire();
	m_mouse->Release();
	m_mouse = nullptr;
	
	m_directInput->Release();
	m_directInput = nullptr;

}

void beMouse::Update(float dt)
{
	m_lastState = m_currentState;
	
	auto result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_currentState.mouseState);
	if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
	{
		result = m_mouse->Acquire();
		if (result >= 0)
		{
			result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_currentState.mouseState);
		}
	}
	if (result < 0)
	{
		return;
	}
	
	m_currentState.x += m_currentState.mouseState.lX;
	m_currentState.y += m_currentState.mouseState.lY;

	// Clamp
	if (m_currentState.x < 0) { m_currentState.x = 0; }
	if (m_currentState.y < 0) { m_currentState.y = 0; }
	if (m_currentState.x > m_windowWidth)  { m_currentState.x = m_windowWidth; }
	if (m_currentState.y > m_windowHeight) { m_currentState.y = m_windowHeight; }
}

static inline bool isDown(const DIMOUSESTATE2& mouseState, beMouse::Button button)
{
	u8 buttonState = mouseState.rgbButtons[button];
	return (buttonState & 0x80) != 0;
}

bool beMouse::IsPressed(Button button) const
{
	return isDown(m_currentState.mouseState, button) && !isDown(m_lastState.mouseState, button);
}

bool beMouse::IsReleased(Button button) const
{
	return isDown(m_lastState.mouseState, button) && !isDown(m_currentState.mouseState, button);
}

bool beMouse::IsDown(Button button) const
{
	return isDown(m_currentState.mouseState, button);
}

