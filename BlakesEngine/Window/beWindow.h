#include "windows.h"
#include "Core/beString.h"

class beWindow
{
	public:
		static beWindow* Create(const beString& windowName, int windowHeight, int windowWidth, bool fullscreen);
		static void Destroy(beWindow* window);

	private:
		class Impl;
		Impl* impl;
};