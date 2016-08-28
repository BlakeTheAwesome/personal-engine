#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePimpl.h"

class beSystemEventManager;

class beWindow
{
	PIMPL_DECLARE(beWindow, beSystemEventManager* systemEventManager, void* hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen);
	
	void* GetHInstance() const;
	void* GetHWnd() const;
	int GetWidth() const;
	int GetHeight() const;
};