module;
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Platform/beWindows.h"
export module beWindow;

import beSystemEventManager;

export class beWindow
{
	public:
	beWindow(beSystemEventManager* systemEventManager, HINSTANCE hInstance, const beStringView& windowName, int windowWidth, int windowHeight, bool fullscreen);
	~beWindow();

	HINSTANCE GetHInstance() const;
	HWND GetHWnd() const;

	// Client rect
	int GetWidth() const;
	int GetHeight() const;

	// Includes menus/borders, etc.
	int GetWindowWidth() const;
	int GetWindowHeight() const;

	int GetX() const;
	int GetY() const;

	private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	beSystemEventManager* m_systemEventManager = nullptr;
	HWND m_hWnd{nullptr};
	HINSTANCE m_hInstance{nullptr};
	int m_width = 0;
	int m_height = 0;
	int m_clientRectWidth = 0;
	int m_clientRectHeight = 0;
	int m_x = 0;
	int m_y = 0;
};