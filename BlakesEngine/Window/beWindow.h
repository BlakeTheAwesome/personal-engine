#include "BlakesEngine\Core/beString.h"
#include "BlakesEngine\Core/bePimpl.h"

class beWindow
{
	PIMPL_DECLARE(beWindow, void* hInstance, const beString& windowName, int windowWidth, int windowHeight, bool fullscreen);
	
	void* GetHWnd() const;
	int GetWidth() const;
	int GetHeight() const;
};