#include "Core/beString.h"
#include "Core/bePimpl.h"

class beWindow
{
	BE_PIMPL_CREATE(beWindow, void* hInstance, const beString& windowName, int windowHeight, int windowWidth, bool fullscreen);
	
	void* GetHWnd() const;
};