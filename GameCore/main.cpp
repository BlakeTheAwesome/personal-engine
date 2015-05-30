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
#include "BlakesEngine\Camera\beFlightCamera.h"
#include "BlakesEngine\Rendering\beDebugWorld.h"
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
	auto window = PIMPL_NEW(beWindow)(&hInstance, windowName, 1024, 768, false);
	auto renderInterface = PIMPL_NEW(beRenderInterface)();
	renderInterface->Init(window, 0.01f, 100.00f, true);

	beFrameTimer frameTimer;
	frameTimer.LimitFPS(120);

	beFlightCamera camera;
	beModel model;
	beTexture texture;
	beShaderColour colourShader;
	beShaderTexture textureShader;
	auto debugWorld = PIMPL_NEW(beDebugWorld)();
	//model.Init(renderInterface);
	//model.InitWithFilename(renderInterface, "cube.obj");
	//model.InitWithFilename(renderInterface, "cube2.obj");
	model.InitWithFilename(renderInterface, "teapot.obj");
	texture.Init(renderInterface, beWString(L"boar.dds"));
	colourShader.Init(renderInterface, beWString(L"Colour.ps"), beWString(L"Colour.vs"));
	textureShader.Init(renderInterface, beWString(L"Texture.ps"), beWString(L"Texture.vs"));
	debugWorld->Init(renderInterface, true);

	beGamepad gamepad;
	gamepad.Init(0);
	camera.AttachGamepad(&gamepad);

	bool go = true;
	MSG msg = {0};
	while(go)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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
			camera.Update(dt.ToSeconds());
			renderInterface->BeginFrame();

			colourShader.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			textureShader.SetShaderParameters(renderInterface, camera.GetViewMatrix(), camera.GetPosition());
			
			model.Render(renderInterface);
			textureShader.Render(renderInterface, model.GetIndexCount(), texture.GetTexture());
			//colourShader.Render(renderInterface, model.GetIndexCount(), 0);
			
			debugWorld->Render(renderInterface, &colourShader);
			renderInterface->EndFrame();
			//bePRINTF("timeSinceStart %3.3f, dt:%3.3f", (float)beClock::GetSecondsSinceStart(), dt.ToSeconds());

		}
	}

	camera.DetachGamepad();
	gamepad.Deinit();

	debugWorld->Deinit();
	textureShader.Deinit();
	colourShader.Deinit();
	texture.Deinit();
	model.Deinit();
	renderInterface->Deinit();
	PIMPL_DELETE(renderInterface);
	PIMPL_DELETE(window);

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}