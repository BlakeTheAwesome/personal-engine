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
#include "BlakesEngine\Rendering\beBitmap.h"
#include "BlakesEngine\Rendering\beModel.h"
#include "BlakesEngine\Rendering\beTexture.h"
#include "BlakesEngine\Shaders\beShaderColour.h"
#include "BlakesEngine\Shaders\beShaderTexture.h"
#include "BlakesEngine\Shaders\beShaderTexture2d.h"
#include "BlakesEngine\Shaders\beShaderLitTexture.h"

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
	beModel model1;
	beModel model2;
	beModel model3;
	beModel model4;
	beBitmap bitmap1;
	beTexture texture;
	beShaderColour colourShader;
	beShaderTexture textureShader;
	beShaderTexture2d textureShader2d;
	beShaderLitTexture litTextureShader;
	auto debugWorld = PIMPL_NEW(beDebugWorld)();
	
	model1.Init(renderInterface);
	model2.InitWithFilename(renderInterface, "cube.obj");
	model3.InitWithFilename(renderInterface, "cube2.obj");
	model4.InitWithFilename(renderInterface, "teapot.obj");

	bitmap1.Init(renderInterface, 512, 512);

	texture.Init(renderInterface, beWString(L"boar.dds"));
	colourShader.Init(renderInterface, beWString(L"Colour.ps"), beWString(L"Colour.vs"));
	textureShader.Init(renderInterface, beWString(L"Texture.ps"), beWString(L"Texture.vs"));
	textureShader2d.Init(renderInterface, beWString(L"Texture.ps"), beWString(L"Texture2d.vs"));
	litTextureShader.Init(renderInterface, beWString(L"Light.ps"), beWString(L"Light.vs"));
	debugWorld->Init(renderInterface);

	beGamepad gamepad;
	gamepad.Init(0);
	camera.AttachGamepad(&gamepad);

	const int numShaders = 3;
	const int numModels = 4;
	int shaderToUse = 0;
	int modelToUse = 2;
	bool renderAxes = true;

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
			if (gamepad.GetButtonReleased(beGamepad::Y))
			{
				shaderToUse++;
				shaderToUse %= numShaders;
			}
			if (gamepad.GetButtonReleased(beGamepad::X))
			{
				modelToUse++;
				modelToUse %= numModels;
			}
			if (gamepad.GetButtonReleased(beGamepad::Select))
			{
				renderAxes = !renderAxes;
			}

			renderInterface->Update(dt.ToSeconds());
			camera.Update(dt.ToSeconds());
			renderInterface->BeginFrame();

			colourShader.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			textureShader.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			textureShader2d.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			litTextureShader.SetShaderParameters(renderInterface, camera.GetViewMatrix(), camera.GetPosition());
			
			beModel* modelToRender = nullptr;
			switch (modelToUse)
			{
				case 0: modelToRender = &model1; break;
				case 1: modelToRender = &model2; break;
				case 2: modelToRender = &model3; break;
				case 3: modelToRender = &model4; break;
			}

			modelToRender->Render(renderInterface);

			switch (shaderToUse)
			{
				case 0: litTextureShader.Render(renderInterface, modelToRender->GetIndexCount(), texture.GetTexture()); break;
				case 1: textureShader.Render(renderInterface, modelToRender->GetIndexCount(), texture.GetTexture()); break;
				case 2: colourShader.Render(renderInterface, modelToRender->GetIndexCount(), 0); break;
			}

			debugWorld->SetRenderAxes(renderAxes);
			debugWorld->Render(renderInterface, &colourShader);

			renderInterface->DisableZBuffer();
			bitmap1.Render(renderInterface);
			textureShader2d.Render(renderInterface, bitmap1.GetIndexCount(), texture.GetTexture());
			renderInterface->EnableZBuffer();

			renderInterface->EndFrame();
			//bePRINTF("timeSinceStart %3.3f, dt:%3.3f", (float)beClock::GetSecondsSinceStart(), dt.ToSeconds());

		}
	}

	camera.DetachGamepad();
	gamepad.Deinit();

	debugWorld->Deinit();
	litTextureShader.Deinit();
	textureShader2d.Deinit();
	textureShader.Deinit();
	colourShader.Deinit();
	texture.Deinit();
	bitmap1.Deinit();
	model4.Deinit();
	model3.Deinit();
	model2.Deinit();
	model1.Deinit();
	renderInterface->Deinit();
	PIMPL_DELETE(renderInterface);
	PIMPL_DELETE(window);

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}