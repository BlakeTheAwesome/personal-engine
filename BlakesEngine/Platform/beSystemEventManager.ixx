module;
#include "BlakesEngine/Platform/beWindows.h"

export module beSystemEventManager;

import beTypes;
import bePrimitiveType;
import beVector;

export class beSystemEventManager
{
	public:
	beSystemEventManager();
	~beSystemEventManager();
	using CallbackId = bePrimitiveType<u8, 0xff>;

	void Update();

	using tCallbackWinPump = void(*)(const MSG& msg, void* userdata);
	CallbackId RegisterCallbackWin32Pump(void* userdata, tCallbackWinPump);
	void DeregisterCallbackWin32Pump(CallbackId id);

	using tCallbackWinProc = bool(*)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* userdata, LRESULT* result);
	CallbackId RegisterCallbackWinProc(void* userdata, tCallbackWinProc);
	void DeregisterCallbackWinProc(CallbackId id);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	struct CallbackWin32Pump { void* userdata; tCallbackWinPump cb; CallbackId id; };
	beFixedVector<CallbackWin32Pump, 16> m_callbackWin32Pump;

	struct CallbackWinProc { void* userdata; tCallbackWinProc cb; CallbackId id; };
	beFixedVector<CallbackWinProc, 16> m_callbackWinProc;

	CallbackId m_nextCallbackId{0};
};

