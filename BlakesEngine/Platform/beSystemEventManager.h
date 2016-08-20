#pragma once
#include "BlakesEngine/Core/beTypes.h"
#include "BlakesEngine/Core/bePrimitiveType.h"
#include "BlakesEngine/Core/bePimpl.h"

#include "BlakesEngine/Platform/beWindows.h"


class beSystemEventManager
{
	PIMPL_DECLARE(beSystemEventManager);

	PrimitiveType(CallbackId, u8);

	void Update();

	typedef void (*tCallbackWinPump)(const MSG& msg, void* userdata);
	CallbackId RegisterCallbackWin32Pump(void* userdata, tCallbackWinPump);
	void DeregisterCallbackWin32Pump(CallbackId id);

	typedef bool (*tCallbackWinProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* userdata, LRESULT* result);
	CallbackId RegisterCallbackWinProc(void* userdata, tCallbackWinProc);
	void DeregisterCallbackWinProc(CallbackId id);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

