#include <windows.h>

#include "BlakesEngine\Core\beString.h"
#include "BlakesEngine\Core\bePrintf.h"
#include "BlakesEngine\Core\beTypeTests.h"
#include "BlakesEngine\Window\beWindow.h"

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

	beString windowName("TestWindow");
	beWindow* window = beWindow::Create(hInstance, windowName, 1024, 768, false);

	beWindow::Destroy(window);

	//int frameNumber = 0;
	MSG msg;
	while(true)
	{
		if(&msg, NULL, 0, 0, PM_REMOVE)
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			break;
		}

		
		//bePRINTF("frame %d", frameNumber++);
	}

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}