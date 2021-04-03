module;

#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Platform/beWindows.h"

module beSystemEventManager;

import beVector;

static beSystemEventManager* s_instance = nullptr;

beSystemEventManager::beSystemEventManager()
{
	BE_ASSERT(!s_instance);
	s_instance = this;
}

beSystemEventManager::~beSystemEventManager()
{
	BE_ASSERT(m_callbackWin32Pump.Count() == 0);
	BE_ASSERT(m_callbackWinProc.Count() == 0);
	
	BE_ASSERT(s_instance);
	s_instance = nullptr;
}

void beSystemEventManager::Update()
{
	MSG msg{};
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// translate keystroke messages into the right format
		TranslateMessage(&msg);

		// send the message to the WindowProc function
		DispatchMessage(&msg);
		//LOG("recieved message:0x%08x", msg.message);
		
		for (const auto& callback : m_callbackWin32Pump)
		{
			callback.cb(msg, callback.userdata);
		}
	}
}

beSystemEventManager::CallbackId beSystemEventManager::RegisterCallbackWin32Pump(void* userdata, tCallbackWinPump cb)
{
	beSystemEventManager::CallbackId id = m_nextCallbackId++;
	m_callbackWin32Pump.AddNew(userdata, cb, id);
	return id;
}

void beSystemEventManager::DeregisterCallbackWin32Pump(beSystemEventManager::CallbackId id)
{
	for (auto& cb : m_callbackWin32Pump)
	{
		if (cb.id == id)
		{
			m_callbackWin32Pump.QuickRemove(&cb);
			return;
		}
	}

	BE_ASSERT(false);
}


beSystemEventManager::CallbackId beSystemEventManager::RegisterCallbackWinProc(void* userdata, tCallbackWinProc cb)
{
	beSystemEventManager::CallbackId id = m_nextCallbackId++;
	m_callbackWinProc.AddNew(userdata, cb, id);
	return id;
}

void beSystemEventManager::DeregisterCallbackWinProc(beSystemEventManager::CallbackId id)
{
	for (auto& cb : m_callbackWinProc)
	{
		if (cb.id == id)
		{
			m_callbackWinProc.QuickRemove(&cb);
			return;
		}
	}

	BE_ASSERT(false);
}

LRESULT CALLBACK beSystemEventManager::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//beSystemEventManager* pThis = (beSystemEventManager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	//BE_ASSERT(pThis == s_instance);
	
	LRESULT result;
	for (const auto& callback : s_instance->m_callbackWinProc)
	{
		if (callback.cb(hWnd, message, wParam, lParam, callback.userdata, &result))
		{
			return result;
		}
	}

	// Handle any messages the game didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}