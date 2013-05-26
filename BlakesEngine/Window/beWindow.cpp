#include "BlakesEngine_PCH.h"
#include "beWindow.h"


class beWindow::Impl
{
	public:
		Impl(HINSTANCE hInstance,const beString& windowName, int windowWidth, int windowHeight, bool fullscreen);
		~Impl();

	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		HWND m_hWnd;
};

beWindow* beWindow::Create(HINSTANCE hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
{
	beWindow* self = (beWindow*)malloc(sizeof(beWindow) + sizeof(Impl));
	void* impl = self+1;
	new(impl) Impl(hInstance, windowName, windowWidth, windowHeight, fullscreen);
	self->impl = (Impl*)impl;
	return self;
}

void beWindow::Destroy(beWindow* window)
{
	window->impl->~Impl();
	free(window);
}

beWindow::Impl::Impl(HINSTANCE hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen)
: m_hWnd(NULL)
{
	std::wstring wideWindowName;
	wideWindowName.assign(windowName.begin (), windowName.end ());

	WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
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
}


// this is the main message handler for the program
LRESULT CALLBACK beWindow::Impl::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Impl* pThis = (Impl*)lParam;

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