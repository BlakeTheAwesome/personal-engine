module;

#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Platform/beWindows.h"
#include "BlakesEngine/External/DirectXTK/Keyboard.h"

#include <mutex>

module beKeyboard;

import beSystemEventManager;

DirectX::Keyboard s_keyboard;
static int s_instanceCount = 0;
static beSystemEventManager::CallbackId s_systemCallbackId;
static std::mutex s_mutex;

beKeyboard::~beKeyboard()
{
	BE_ASSERT(!m_systemEventManager); 
}

void beKeyboard::Init(beSystemEventManager* systemEventManager)
{
	m_systemEventManager = systemEventManager;

	std::lock_guard<decltype(s_mutex)> lock(s_mutex);
	if (++s_instanceCount == 1)
	{
		s_systemCallbackId = systemEventManager->RegisterCallbackWinProc(nullptr, [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* userdata, LRESULT* result) -> bool {
			s_keyboard.ProcessMessage(message, wParam, lParam);
			return false;
		});
	}
}

void beKeyboard::Deinit()
{
	std::lock_guard<decltype(s_mutex)> lock(s_mutex);
	if (--s_instanceCount == 0)
	{
		m_systemEventManager->DeregisterCallbackWinProc(s_systemCallbackId);
	}
	m_systemEventManager = nullptr;
}

void beKeyboard::Update(float dt)
{
	m_tracker.Update(s_keyboard.GetState());
}

bool beKeyboard::IsPressed(Button button) const
{
	return m_tracker.IsKeyPressed(button);
}

bool beKeyboard::IsReleased(Button button) const
{
	return m_tracker.IsKeyReleased(button);
}

bool beKeyboard::IsDown(Button button) const
{
	return m_tracker.GetLastState().IsKeyDown(button);
}

