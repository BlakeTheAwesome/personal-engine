#include "BlakesEngine/bePCH.h"
#include "beWindow.h"

#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Platform/beSystemEventManager.h"
#include "BlakesEngine/Platform/beWindows.h"

PIMPL_DATA(beWindow, beSystemEventManager* systemEventManager, void* hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static Impl* s_staticInstance;
		beSystemEventManager* m_systemEventManager;
		HWND m_hWnd{nullptr};
		HINSTANCE m_hInstance{nullptr};
		int m_width = 0;
		int m_height = 0;
		int m_clientRectWidth = 0;
		int m_clientRectHeight = 0;
		int m_x = 0;
		int m_y = 0;
PIMPL_DATA_END

beWindow::Impl* beWindow::Impl::s_staticInstance = nullptr;

PIMPL_CONSTRUCT_ARGS(beWindow, beSystemEventManager* systemEventManager, void* _hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
PIMPL_CONSTRUCT_ARGS_VARS(beWindow, systemEventManager, _hInstance, windowName, windowWidth, windowHeight, fullscreen)
PIMPL_CONSTRUCT_ARGS_BODY(beWindow, beSystemEventManager* systemEventManager, void* _hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
: m_systemEventManager(systemEventManager)
, m_width(windowWidth)
, m_height(windowHeight)
{
	auto hInstance = *(HINSTANCE*)_hInstance;
	m_hInstance = hInstance;
	std::wstring wideWindowName;
	wideWindowName.assign(windowName.begin (), windowName.end ());

	WNDCLASSEX wc = {0};
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
	RECT clientRect{0};
	if (!GetClientRect(m_hWnd, &clientRect))
	{
		BE_ASSERT(false);
	}
	m_clientRectWidth = clientRect.right - clientRect.left;
	m_clientRectHeight = clientRect.bottom - clientRect.top;
	s_staticInstance = this;
}

PIMPL_DESTROY(beWindow)
{
	s_staticInstance = nullptr;
	DestroyWindow(m_hWnd);
}

void * beWindow::GetHInstance() const
{
	return &self.m_hInstance;
}

void* beWindow::GetHWnd() const
{
	return &self.m_hWnd;
}

int beWindow::GetWidth() const
{
	return self.m_clientRectWidth;
}

int beWindow::GetHeight() const
{
	return self.m_clientRectHeight;
}

int beWindow::GetWindowWidth() const
{
	return self.m_width;
}

int beWindow::GetWindowHeight() const
{
	return self.m_height;
}

int beWindow::GetX() const
{
	return self.m_x;
}

int beWindow::GetY() const
{
	return self.m_y;
}

LRESULT CALLBACK beWindow::Impl::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			Impl* self = s_staticInstance;
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