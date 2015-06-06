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
#include "BlakesEngine\Rendering\beFont.h"
#include "BlakesEngine\Shaders\beShaderColour.h"
#include "BlakesEngine\Shaders\beShaderTexture.h"
#include "BlakesEngine\Shaders\beShaderTexture2d.h"
#include "BlakesEngine\Shaders\beShaderLitTexture.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   PSTR lpCmdLine,
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
	beFont font;
	//beTexture texture;
	beShaderColour colourShader;
	beShaderTexture textureShader;
	beShaderTexture2d textureShader2d;
	beShaderLitTexture litTextureShader;
	auto debugWorld = PIMPL_NEW(beDebugWorld)();
	
	font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));
	model1.Init(renderInterface, beWString(L"boar.dds"));
	model2.InitWithFilename(renderInterface, "cube.obj", beWString(L"seafloor.dds"));
	model3.InitWithFilename(renderInterface, "cube2.obj", beWString(L"seafloor.dds"));
	model4.InitWithFilename(renderInterface, "teapot.obj", beWString(L"seafloor.dds"));

	bitmap1.Init(renderInterface, 512, 512, beWString(L"boar.dds"));

	//texture.Init(renderInterface, beWString(L"boar.dds"));
	colourShader.Init(renderInterface, beWString(L"Colour_p.cso"), beWString(L"Colour_v.cso"));
	textureShader.Init(renderInterface, beWString(L"Texture_p.cso"), beWString(L"Texture_v.cso"));
	textureShader2d.Init(renderInterface, beWString(L"Texture_p.cso"), beWString(L"Texture2d_v.cso"));
	litTextureShader.Init(renderInterface, beWString(L"Light_p.cso"), beWString(L"Light_v.cso"));
	debugWorld->Init(renderInterface);

	beGamepad gamepad;
	gamepad.Init(0);
	camera.AttachGamepad(&gamepad);

	const int numShaders = 3;
	const int numModels = 5;
	int shaderToUse = 0;
	int modelToUse = 3;
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
			if (gamepad.GetButtonReleased(beGamepad::Up))
			{
				Vec2 bitmapPosition = bitmap1.GetPosition();
				bitmap1.SetPosition(bitmapPosition.x, bitmapPosition.y + 10.f);
			}
			if (gamepad.GetButtonReleased(beGamepad::Down))
			{
				Vec2 bitmapPosition = bitmap1.GetPosition();
				bitmap1.SetPosition(bitmapPosition.x, bitmapPosition.y - 10.f);
			}
			if (gamepad.GetButtonReleased(beGamepad::Left))
			{
				Vec2 bitmapPosition = bitmap1.GetPosition();
				bitmap1.SetPosition(bitmapPosition.x - 10.f, bitmapPosition.y);
			}
			if (gamepad.GetButtonReleased(beGamepad::Right))
			{
				Vec2 bitmapPosition = bitmap1.GetPosition();
				bitmap1.SetPosition(bitmapPosition.x + 10.f, bitmapPosition.y);
			}

			renderInterface->Update(dt.ToSeconds());
			camera.Update(dt.ToSeconds());
			renderInterface->BeginFrame();

			
			beModel* modelToRender = nullptr;
			switch (modelToUse)
			{
				case 0: modelToRender = nullptr; break;
				case 1: modelToRender = &model1; break;
				case 2: modelToRender = &model2; break;
				case 3: modelToRender = &model3; break;
				case 4: modelToRender = &model4; break;
			}

			if (modelToRender)
			{
				modelToRender->Render(renderInterface);

				switch (shaderToUse)
				{
					case 0:
						litTextureShader.SetShaderParameters(renderInterface, camera.GetViewMatrix(), camera.GetPosition());
						litTextureShader.Render(renderInterface, modelToRender->GetIndexCount(), modelToRender->GetTexture());
					break;
					case 1:
						textureShader.SetShaderParameters(renderInterface, camera.GetViewMatrix());
						textureShader.Render(renderInterface, modelToRender->GetIndexCount(), modelToRender->GetTexture());
					break;
					case 2:
						colourShader.SetShaderParameters(renderInterface, camera.GetViewMatrix());
						colourShader.Render(renderInterface, modelToRender->GetIndexCount(), 0);
					break;
				}
			}

			//debugWorld->SetRenderAxes(renderAxes);
			//debugWorld->Render(renderInterface, &colourShader);

			renderInterface->DisableZBuffer();
			textureShader2d.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			bitmap1.Render(renderInterface);
			textureShader2d.Render(renderInterface, bitmap1.GetIndexCount(), bitmap1.GetTexture());
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
	//texture.Deinit();
	bitmap1.Deinit();
	model4.Deinit();
	model3.Deinit();
	model2.Deinit();
	model1.Deinit();
	font.Deinit();
	renderInterface->Deinit();
	PIMPL_DELETE(renderInterface);
	PIMPL_DELETE(window);

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}