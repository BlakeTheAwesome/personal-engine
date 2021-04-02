#pragma once
#include "BlakesEngine/Core/bePimpl.h"
#include "BlakesEngine/Platform/beWindows.h"

import beTypes;
import bePrimitiveType;

class beSystemEventManager
{
	PIMPL_DECLARE(beSystemEventManager);

	using CallbackId = bePrimitiveType<u8, 0xff>;

	void Update();

	using tCallbackWinPump = void(*)(const MSG& msg, void* userdata);
	CallbackId RegisterCallbackWin32Pump(void* userdata, tCallbackWinPump);
	void DeregisterCallbackWin32Pump(CallbackId id);

	using tCallbackWinProc = bool(*)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* userdata, LRESULT* result);
	CallbackId RegisterCallbackWinProc(void* userdata, tCallbackWinProc);
	void DeregisterCallbackWinProc(CallbackId id);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

