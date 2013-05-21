#include "BlakesEngine_PCH.h"
#include "beWindow.h"

class beWindow::Impl
{
	public:
		Impl(const beString& windowName, int windowHeight, int windowWidth, bool fullscreen);
		~Impl();

		int i;
};

beWindow* beWindow::Create(const beString& windowName, int windowHeight, int windowWidth, bool fullscreen)
{
	beWindow* self = (beWindow*)malloc(sizeof(beWindow) + sizeof(Impl));
	void* impl = self+1;
	new(impl) Impl(windowName, windowHeight, windowWidth, fullscreen);
	self->impl = (Impl*)impl;
	return self;
}

void beWindow::Destroy(beWindow* window)
{
	window->impl->~Impl();
	free(window);
}