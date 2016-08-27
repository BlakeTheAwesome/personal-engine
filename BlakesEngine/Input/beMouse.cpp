#include "BlakesEngine/bePCH.h"
#include "beMouse.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Platform/beSystemEventManager.h"
#include "BlakesEngine/Window/beWindow.h"

#include <mutex>

DirectX::Mouse s_mouse;
static int s_instanceCount = 0;
static beSystemEventManager::CallbackId s_systemCallbackId;
static HWND s_hWnd;
static std::mutex s_mutex;

beMouse::~beMouse()
{
	BE_ASSERT(!m_systemEventManager); 
}

void beMouse::Init(beSystemEventManager* systemEventManager, const beWindow* window)
{
	m_systemEventManager = systemEventManager;
	s_hWnd = *(HWND*)window->GetHWnd();

	std::lock_guard<decltype(s_mutex)> lock(s_mutex);
	if (++s_instanceCount == 1)
	{
		s_systemCallbackId = systemEventManager->RegisterCallbackWinProc(nullptr, [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* userdata, LRESULT* result) -> bool {
			if (hWnd == s_hWnd)
			{
				s_mouse.ProcessMessage(message, wParam, lParam);
			}
			return false;
		});

		s_mouse.SetWindow(s_hWnd);
		s_mouse.SetMode(DirectX::Mouse::MODE_RELATIVE);
		//s_mouse.SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	}
}

void beMouse::Deinit()
{
	std::lock_guard<decltype(s_mutex)> lock(s_mutex);
	if (--s_instanceCount == 0)
	{
		m_systemEventManager->DeregisterCallbackWinProc(s_systemCallbackId);
	}
	m_systemEventManager = nullptr;
}

void beMouse::Update(float dt)
{
	m_tracker.Update(s_mouse.GetState());
}

bool beMouse::IsPressed(Button button) const
{
	switch (button)
	{
		case Button::LeftButton: return m_tracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
		case Button::MiddleButton: return m_tracker.middleButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
		case Button::RightButton: return m_tracker.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
		case Button::XButton1: return m_tracker.xButton1 == DirectX::Mouse::ButtonStateTracker::PRESSED;
		case Button::XButton2: return m_tracker.xButton2 == DirectX::Mouse::ButtonStateTracker::PRESSED;
	}
	BE_ASSERT(false);
	return false;
}

bool beMouse::IsReleased(Button button) const
{
	switch (button)
	{
		case Button::LeftButton: return m_tracker.leftButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
		case Button::MiddleButton: return m_tracker.middleButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
		case Button::RightButton: return m_tracker.rightButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
		case Button::XButton1: return m_tracker.xButton1 == DirectX::Mouse::ButtonStateTracker::RELEASED;
		case Button::XButton2: return m_tracker.xButton2 == DirectX::Mouse::ButtonStateTracker::RELEASED;
	}
	BE_ASSERT(false);
	return false;
}

bool beMouse::IsDown(Button button) const
{
	switch (button)
	{
		case Button::LeftButton: return m_tracker.leftButton == DirectX::Mouse::ButtonStateTracker::HELD;
		case Button::MiddleButton: return m_tracker.middleButton == DirectX::Mouse::ButtonStateTracker::HELD;
		case Button::RightButton: return m_tracker.rightButton == DirectX::Mouse::ButtonStateTracker::HELD;
		case Button::XButton1: return m_tracker.xButton1 == DirectX::Mouse::ButtonStateTracker::HELD;
		case Button::XButton2: return m_tracker.xButton2 == DirectX::Mouse::ButtonStateTracker::HELD;
	}
	BE_ASSERT(false);
	return false;
}

int beMouse::GetX() const
{
	return m_tracker.GetLastState().x;
}

int beMouse::GetY() const
{
	return m_tracker.GetLastState().y;
}

int beMouse::GetScrollWheelValue() const
{
	return m_tracker.GetLastState().scrollWheelValue;
}

