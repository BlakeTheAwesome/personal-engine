#include "bePCH.h"
#include "beWindow.h"

#include "Core/beMacros.h"
#include <windows.h>

class beWindow::Impl
{
	public:
		Impl(HINSTANCE hInstance,const beString& windowName, int windowWidth, int windowHeight, bool fullscreen);
		~Impl();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		HWND m_hWnd;
		int m_width;
		int m_height;
};

BE_PIMPL_CPP_DECLARE(beWindow, void* hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
{
	BE_PIMPL_CPP_BODY(beWindow, *(HINSTANCE*)hInstance, windowName, windowWidth, windowHeight, fullscreen);
}

beWindow::Impl::Impl(HINSTANCE hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
: m_hWnd(NULL)
, m_width(windowWidth)
, m_height(windowHeight)
{
	std::wstring wideWindowName;
	wideWindowName.assign(windowName.begin (), windowName.end ());

	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW; // Enable to put inital colour back in
	wc.lpszClassName = L"beWindowClass";

	// register the window class
	RegisterClassEx(&wc);

	// create the window and use the result as the handle
	m_hWnd = CreateWindowEx(NULL,
							L"beWindowClass",
							wideWindowName.c_str(),
							WS_OVERLAPPEDWINDOW, // window style - add noresize etc
							448,    // x
							156,    // y
							windowWidth, // w
							windowHeight, // h
							NULL,    // parent window
							NULL,    // we aren't using menus, NULL
							hInstance,    // application handle
							this);    // callback thing?

	// display the window on the screen
	ShowWindow(m_hWnd, SW_SHOW);
}

beWindow::Impl::~Impl()
{
	DestroyWindow(m_hWnd);
}

void* beWindow::GetHWnd() const
{
	return &m_impl->m_hWnd;
}

int beWindow::GetWidth() const
{
	return m_impl->m_width;
}

int beWindow::GetHeight() const
{
	return m_impl->m_height;
}

// this is the main message handler for the program
LRESULT CALLBACK beWindow::Impl::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Impl* pThis = (Impl*)lParam;

    // sort through and find what code to run for the message given
    switch(message)
    {
        // this message is read when the window is closed
        case WM_DESTROY:
            {
                // close the application entirely
                PostQuitMessage(0);
                return 0;
            } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}