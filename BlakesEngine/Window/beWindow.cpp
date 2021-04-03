module;
#include "BlakesEngine/bePCH.h"

#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Platform/beWindows.h"
module beWindow;

import beSystemEventManager;

static beWindow* s_staticInstance = nullptr;

beWindow::beWindow(beSystemEventManager* systemEventManager, HINSTANCE hInstance, const beStringView& windowName, int windowWidth, int windowHeight, bool fullscreen)
: m_systemEventManager(systemEventManager)
, m_width(windowWidth)
, m_height(windowHeight)
, m_hInstance(hInstance)
{
	std::wstring wideWindowName;
	wideWindowName.assign(windowName.begin(), windowName.end());

	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszClassName = L"beWindowClass";

	// register the window class
	RegisterClassEx(&wc);

	m_x = 120;
	m_y = 16;

	// create the window and use the result as the handle
	m_hWnd = CreateWindowEx(0,
							L"beWindowClass",
							wideWindowName.c_str(),
							WS_OVERLAPPEDWINDOW, // window style - add noresize etc
							m_x,    // x
							m_y,    // y
							windowWidth, // w
							windowHeight, // h
							nullptr,    // parent window
							nullptr,    // we aren't using menus, nullptr
							hInstance,    // application handle
							this);

	// display the window on the screen
	ShowWindow(m_hWnd, SW_SHOW);
	RECT clientRect{};
	if (!GetClientRect(m_hWnd, &clientRect))
	{
		BE_ASSERT(false);
	}
	m_clientRectWidth = clientRect.right - clientRect.left;
	m_clientRectHeight = clientRect.bottom - clientRect.top;

	BE_ASSERT(!s_staticInstance);
	s_staticInstance = this;
}

beWindow::~beWindow()
{
	s_staticInstance = nullptr;
	DestroyWindow(m_hWnd);
}

HINSTANCE beWindow::GetHInstance() const
{
	return m_hInstance;
}

HWND beWindow::GetHWnd() const
{
	return m_hWnd;
}

int beWindow::GetWidth() const
{
	return m_clientRectWidth;
}

int beWindow::GetHeight() const
{
	return m_clientRectHeight;
}

int beWindow::GetWindowWidth() const
{
	return m_width;
}

int beWindow::GetWindowHeight() const
{
	return m_height;
}

int beWindow::GetX() const
{
	return m_x;
}

int beWindow::GetY() const
{
	return m_y;
}

LRESULT CALLBACK beWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	//	case WM_INITDIALOG:
	//	{
	//		auto pThis = (Impl*)lParam;
	//		SetWindowLongPtr(hWnd, DWLP_USER, (LONG_PTR)pThis->m_systemEventManager);
	//		return TRUE;
	//	}
	//	break;

		case WM_MOVE:
		{
			beWindow* self = s_staticInstance;
			if (self)
			{
				self->m_x = (int)(short)LOWORD(lParam);   // horizontal position 
				self->m_y = (int)(short)HIWORD(lParam);   // vertical position 
			}
		}
		break;
	}

	return beSystemEventManager::WindowProc(hWnd, message, wParam, lParam);
}