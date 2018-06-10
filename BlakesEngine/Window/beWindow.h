#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePimpl.h"

class beSystemEventManager;

class beWindow
{
	PIMPL_DECLARE(beWindow, beSystemEventManager* systemEventManager, void* hInstance, const beStringView& windowName, int windowWidth, int windowHeight, bool fullscreen);
	
	void* GetHInstance() const;
	void* GetHWnd() const;

	// Client rect
	int GetWidth() const;
	int GetHeight() const;

	// Includes menus/borders, etc.
	int GetWindowWidth() const;
	int GetWindowHeight() const;

	int GetX() const;
	int GetY() const;
};