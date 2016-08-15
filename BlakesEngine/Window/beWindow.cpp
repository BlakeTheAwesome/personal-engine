#include "bePCH.h"
#include "beWindow.h"

#include "BlakesEngine\Core/beMacros.h"
#include <windows.h>

PIMPL_DATA(beWindow, void* hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		HWND m_hWnd;
		int m_width;
		int m_height;
PIMPL_DATA_END

PIMPL_CONSTRUCT_ARGS(beWindow, void* _hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
PIMPL_CONSTRUCT_ARGS_VARS(beWindow, _hInstance, windowName, windowWidth, windowHeight, fullscreen)
PIMPL_CONSTRUCT_ARGS_BODY(beWindow, void* _hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
	: m_hWnd(nullptr)
, m_width(windowWidth)
, m_height(windowHeight)
{
	auto hInstance = *(HINSTANCE*)_hInstance;
	std::wstring wideWindowName;
	wideWindowName.assign(windowName.begin (), windowName.end ());

	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW; // Enable to put inital colour back in
	wc.lpszClassName = L"beWindowClass";

	// register the window class
	RegisterClassEx(&wc);

	// create the window and use the result as the handle
	m_hWnd = CreateWindowEx(0,
							L"beWindowClass",
							wideWindowName.c_str(),
							WS_OVERLAPPEDWINDOW, // window style - add noresize etc
							448,    // x
							156,    // y
							windowWidth, // w
							windowHeight, // h
							nullptr,    // parent window
							nullptr,    // we aren't using menus, nullptr
							hInstance,    // application handle
							this);    // callback thing?

	// display the window on the screen
	ShowWindow(m_hWnd, SW_SHOW);
}

PIMPL_DESTROY(beWindow)
{
	DestroyWindow(m_hWnd);
}

void* beWindow::GetHWnd() const
{
	return &self.m_hWnd;
}

int beWindow::GetWidth() const
{
	return self.m_width;
}

int beWindow::GetHeight() const
{
	return self.m_height;
}

extern bool GameWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, int* returnCode);

LRESULT CALLBACK beWindow::Impl::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int retCode = 0;
	if (GameWindowProc(hWnd, message, wParam, lParam, &retCode))
	{
		return retCode;
	}

    // Handle any messages the game didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}