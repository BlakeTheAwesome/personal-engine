#include <windows.h>

#include "BlakesEngine\Core\beString.h"
#include "BlakesEngine\Core\bePrintf.h"
#include "BlakesEngine\Core\beMacros.h"
#include "BlakesEngine\Core\beTypeTests.h"
#include "BlakesEngine\Input\beGamepad.h"
#include "BlakesEngine\Time\beFrameTimer.h"
#include "BlakesEngine\Time\beClock.h"
#include "BlakesEngine\Window\beWindow.h"
#include "BlakesEngine\Rendering\beRenderInterface.h"
#include "BlakesEngine\Camera\beCamera.h"
#include "BlakesEngine\Rendering\beModel.h"
#include "BlakesEngine\Rendering\beTexture.h"
#include "BlakesEngine\Shaders\beShaderColour.h"
#include "BlakesEngine\Shaders\beShaderTexture.h"

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
	renderInterface->Init(window, 0.01f, 100.00f, true);

	beFrameTimer frameTimer;
	frameTimer.LimitFPS(120);

	beCamera camera;
	beModel model;
	beTexture texture;
	beShaderColour colourShader;
	beShaderTexture textureShader;
	model.InitWithFilename(renderInterface, "teapot.obj");
	texture.Init(renderInterface, beWString(L"boar.dds"));
	colourShader.Init(renderInterface, beWString(L"Colour.ps"), beWString(L"Colour.vs"));
	textureShader.Init(renderInterface, beWString(L"Texture.ps"), beWString(L"Texture.vs"));

	beGamepad gamepad;
	gamepad.Init(0);

	bool go = true;
	MSG msg = {0};
	while(go)
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
				go = false;
			}
		}


		beTimeValue dt;
		bool doStuff = frameTimer.StepFrame(&dt);

		if (doStuff)
		{
			gamepad.Update(dt.ToSeconds());
			if (gamepad.GetButtonReleased(beGamepad::B))
			{
				go = false;
			}

			renderInterface->Update(dt.ToSeconds());
			camera.Update();
			renderInterface->BeginFrame();
			model.Render(renderInterface);

			//colourShader.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			textureShader.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			textureShader.Render(renderInterface, model.GetIndexCount(), texture.GetTexture());
			renderInterface->EndFrame();
			//bePRINTF("timeSinceStart %3.3f, dt:%3.3f", (float)beClock::GetSecondsSinceStart(), dt.ToSeconds());
		}
	}

	gamepad.Deinit();
	textureShader.Deinit();
	colourShader.Deinit();
	texture.Deinit();
	model.Deinit();
	renderInterface->Deinit();
	BE_SAFE_DESTROY(beRenderInterface, renderInterface);
	BE_SAFE_DESTROY(beWindow, window);

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}