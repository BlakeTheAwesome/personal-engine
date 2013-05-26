#include <windows.h>

#include "BlakesEngine\Core\beString.h"
#include "BlakesEngine\Core\bePrintf.h"
#include "BlakesEngine\Core\beMacros.h"
#include "BlakesEngine\Core\beTypeTests.h"
#include "BlakesEngine\Time\beFrameTimer.h"
#include "BlakesEngine\Time\beClock.h"
#include "BlakesEngine\Window\beWindow.h"
#include "BlakesEngine\Rendering\beRenderInterface.h"

//int main()
//{
//	beTypeTests::RunTypeTests();
//}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	//beTypeTests::RunTypeTests();
	beClock::Initialise();

	beString windowName("TestWindow");
	beWindow* window = beWindow::Create(&hInstance, windowName, 1024, 768, false);
	beRenderInterface* renderInterface = beRenderInterface::Create();
	renderInterface->Init(window->GetHWnd());

	beFrameTimer frameTimer;
	frameTimer.LimitFPS(120);

	MSG msg = {0};
	while(true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);
			//bePRINTF("recieved message:0x%08x", msg.message);
			
			if(msg.message == WM_QUIT)
			{
				break;
			}
		}


		beTimeValue dt;
		bool doStuff = frameTimer.StepFrame(&dt);

		if (doStuff)
		{
			//bePRINTF("timeSinceStart %3.3f, dt:%3.3f", (float)beClock::GetSecondsSinceStart(), dt.ToSeconds());
		}
	}

	renderInterface->Deinit();
	BE_SAFE_DESTROY(beRenderInterface, renderInterface);
	BE_SAFE_DESTROY(beWindow, window);

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}