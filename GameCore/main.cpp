
#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beTypeTests.h"
#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/beKeyboard.h"
#include "BlakesEngine/Input/beMouse.h"
#include "BlakesEngine/Time/beFrameTimer.h"
#include "BlakesEngine/Time/beClock.h"
#include "BlakesEngine/Window/beWindow.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Camera/beFlightCamera.h"
#include "BlakesEngine/Rendering/beDebugWorld.h"
#include "BlakesEngine/Rendering/beBitmap.h"
#include "BlakesEngine/Rendering/beModel.h"
#include "BlakesEngine/Rendering/beTexture.h"
#include "BlakesEngine/Rendering/beFont.h"
#include "BlakesEngine/Shaders/beShaderColour.h"
#include "BlakesEngine/Shaders/beShaderTexture.h"
#include "BlakesEngine/Shaders/beShaderTexture2d.h"
#include "BlakesEngine/Shaders/beShaderLitTexture.h"
#include "BlakesEngine/platform/beSystemEventManager.h"
#include "BlakesEngine/platform/beSystemEventManager.h"

#include "BlakesEngine/Platform/beWindows.h"
#include "BlakesEngine/External/RenderDoc-Manager/RenderDocManager.h"

#ifdef DEBUG
//#define ENABLE_RENDERDOC
#define RENDERDOC_PATH L"d:/Dev/Renderdoc/renderDoc.dll"
#define RENDERDOC_CAPTURE_PATH "d:/temp/renderDoc/capture"
#endif

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   PSTR lpCmdLine,
                   int nCmdShow)
{
	beSystemEventManager* systemEventManager = PIMPL_NEW(beSystemEventManager)();
	defer(PIMPL_DELETE(systemEventManager));

	//beTypeTests::RunTypeTests();
	beClock::Initialise();
	
	beFrameTimer frameTimer;
	frameTimer.LimitFPS(120);
	
	beString windowName("TestWindow");
	auto window = PIMPL_NEW(beWindow)(systemEventManager, &hInstance, windowName, 1024, 768, false);
	
	#ifdef ENABLE_RENDERDOC
		RenderDocManager renderDoc(*(HWND*)window->GetHWnd(), RENDERDOC_PATH, RENDERDOC_CAPTURE_PATH);
	#endif

	auto renderInterface = PIMPL_NEW(beRenderInterface)();
	renderInterface->Init(window, 0.01f, 100.00f, true);
	defer(
		renderInterface->Deinit();
		PIMPL_DELETE(renderInterface);
		PIMPL_DELETE(window);
	);

	auto debugWorld = PIMPL_NEW(beDebugWorld)();
	debugWorld->Init(renderInterface);
	defer(
		debugWorld->Deinit();
		PIMPL_DELETE(debugWorld);
	);
	
	beFont font;
	beModel model1;
	beModel model2;
	beModel model3;
	beModel model4;
	beModel model5;
	font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));
	model1.Init(renderInterface, beWString(L"boar.dds"));
	model2.InitWithFilename(renderInterface, "cube.obj", beWString(L"seafloor.dds"));
	model3.InitWithFilename(renderInterface, "cube2.obj", beWString(L"seafloor.dds"));
	model4.InitWithFilename(renderInterface, "teapot.obj", beWString(L"seafloor.dds"));
	model5.InitWithFilename(renderInterface, "boxes.obj", beWString(L"barrels.dds"));
	defer(
		model5.Deinit();
		model4.Deinit();
		model3.Deinit();
		model2.Deinit();
		model1.Deinit();
		font.Deinit();
	);
	
	beTexture screenGrabTexture;
	screenGrabTexture.InitAsTarget(renderInterface, 256, 256);
	defer(
		screenGrabTexture.FinaliseTarget();
		screenGrabTexture.Deinit();
	);

	beBitmap bitmapTexQuad;
	beBitmap bitmapTextDynamic;
	beBitmap bitmapTextPreRendered;
	beBitmap bitmapScreenGrab;
	bitmapTexQuad.Init(renderInterface, 128, 128, beWString(L"boar.dds"));
	bitmapTexQuad.SetPosition(1024/2-128, 768/2-128);
	bitmapTextDynamic.SetColour(Vec4(0.f, 1.f, 0.8f, 1.f));
	defer(
		bitmapScreenGrab.Deinit();
		bitmapTextPreRendered.Deinit();
		bitmapTextDynamic.Deinit();
		bitmapTexQuad.Deinit();
	);
	
	beShaderColour colourShader;
	beShaderTexture textureShader;
	beShaderTexture2d textureShader2d;
	beShaderLitTexture litTextureShader;
	colourShader.Init(renderInterface, beWString(L"Colour_p.cso"), beWString(L"Colour_v.cso"));
	textureShader.Init(renderInterface, beWString(L"Texture_p.cso"), beWString(L"Texture_v.cso"));
	textureShader2d.Init(renderInterface, beWString(L"Texture_p.cso"), beWString(L"Texture2d_v.cso"));
	litTextureShader.Init(renderInterface, beWString(L"Light_p.cso"), beWString(L"Light_v.cso"));
	defer(
		litTextureShader.Deinit();
		textureShader2d.Deinit();
		textureShader.Deinit();
		colourShader.Deinit();
	);
	
	beKeyboard keyboard;
	keyboard.Init(systemEventManager);
	beMouse mouse;
	mouse.Init(systemEventManager, window);
	beGamepad gamepad;
	gamepad.Init(0);
	beFlightCamera camera;
	camera.AttachGamepad(&gamepad);
	camera.AttachMouse(&mouse);
	defer(
		camera.DetachGamepad();
		gamepad.Deinit();
		mouse.Deinit();
		keyboard.Deinit();
	);


	const int numShaders = 3;
	const int numModels = 6;
	int shaderToUse = 0;
	int modelToUse = 2;
	bool renderAxes = true;
	bool haveWrittenToTexture = false;

	struct Win32Callback {
		bool quit = false;
		int retCode = 0;
	} win32Callback;

	beSystemEventManager::CallbackId win32CallbackId = systemEventManager->RegisterCallbackWin32Pump(&win32Callback, [](const MSG& msg, void* userdata) {
		auto* win32Callback = (Win32Callback*)userdata;
		if(msg.message == WM_QUIT)
		{
			win32Callback->quit = true;
			win32Callback->retCode = (int)msg.wParam;
		}
	});
	defer(systemEventManager->DeregisterCallbackWin32Pump(win32CallbackId));

	struct WinProcCallback {
		HWND hWnd;
	} winProcCallback {*(HWND*)window->GetHWnd()};

	beSystemEventManager::CallbackId winProcCallbackId = systemEventManager->RegisterCallbackWinProc(&winProcCallback, [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* userdata, LRESULT* result) -> bool {
		auto* winProcCallback = (WinProcCallback*)userdata;
		if (hWnd != winProcCallback->hWnd)
		{
			return false;
		}
		switch(message)
		{
			case WM_DESTROY:
			{
				// close the application entirely
				PostQuitMessage(0);
				*result = 0;
				return true;
			} break;
		}

		return false;
	});
	defer(systemEventManager->DeregisterCallbackWinProc(winProcCallbackId));
	




	// ACTUAL MAIN LOOP

	while(!win32Callback.quit)
	{
		systemEventManager->Update();

		beTimeValue dt;
		bool doStuff = frameTimer.StepFrame(&dt);

		if (doStuff)
		{
			keyboard.Update(dt.ToSeconds());
			mouse.Update(dt.ToSeconds());
			gamepad.Update(dt.ToSeconds());
			auto handleInput = [&]()
			{
				if (gamepad.GetButtonReleased(beGamepad::A) || keyboard.IsPressed(beKeyboard::Button::W))
				{
					renderInterface->ToggleWireframe();
				}
				if (gamepad.GetButtonReleased(beGamepad::B) || keyboard.IsPressed(beKeyboard::Button::Escape))
				{
					win32Callback.quit = true;
				}
				if (gamepad.GetButtonReleased(beGamepad::Y))
				{
					shaderToUse++;
					shaderToUse %= numShaders;
				}
				if (gamepad.GetButtonReleased(beGamepad::X) || mouse.IsPressed(beMouse::Button::MiddleButton))
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
					Vec2 bitmapPosition = bitmapTexQuad.GetPosition();
					bitmapTexQuad.SetPosition(bitmapPosition.x, bitmapPosition.y + 10.f);
				}
				if (gamepad.GetButtonReleased(beGamepad::Down))
				{
					Vec2 bitmapPosition = bitmapTexQuad.GetPosition();
					bitmapTexQuad.SetPosition(bitmapPosition.x, bitmapPosition.y - 10.f);
				}
				if (gamepad.GetButtonReleased(beGamepad::Left))
				{
					Vec2 bitmapPosition = bitmapTexQuad.GetPosition();
					bitmapTexQuad.SetPosition(bitmapPosition.x - 10.f, bitmapPosition.y);
				}
				if (gamepad.GetButtonReleased(beGamepad::Right))
				{
					Vec2 bitmapPosition = bitmapTexQuad.GetPosition();
					bitmapTexQuad.SetPosition(bitmapPosition.x + 10.f, bitmapPosition.y);
				}
			};

			handleInput();

			renderInterface->Update(dt.ToSeconds());
			camera.Update(dt.ToSeconds());

			if (!haveWrittenToTexture)
			{
				beTexture writeTexture;
				writeTexture.InitAsTarget(renderInterface, 512, 512);
				defer(
					writeTexture.Deinit();
				);
				haveWrittenToTexture = true;
				writeTexture.SetAsTarget(renderInterface);
				writeTexture.Clear(renderInterface, Vec4(0.f, 0.f, 0.f, 0.0f));
				model4.Render(renderInterface);
				colourShader.SetShaderParameters(renderInterface, camera.GetViewMatrix());
				colourShader.Render(renderInterface, model4.GetIndexCount(), 0);
				
				renderInterface->DisableZBuffer();
				textureShader2d.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			
				//bitmapTexQuad.Render(renderInterface);
				//textureShader2d.Render(renderInterface, bitmapTexQuad.GetIndexCount(), bitmapTexQuad.GetTexture());

				renderInterface->EnableAlpha();
				bitmapTextDynamic.InitText(renderInterface, &font, "Pre-RenderedText", 512.f, 0);
				bitmapTextDynamic.SetPosition((float)(-writeTexture.GetWidth() / 2), (float)(writeTexture.GetHeight() / 2));
				bitmapTextDynamic.Render(renderInterface);
				textureShader2d.Render(renderInterface, bitmapTextDynamic.GetIndexCount(), bitmapTextDynamic.GetTexture());
				renderInterface->DisableAlpha();
				renderInterface->EnableZBuffer();
				renderInterface->RestoreRenderTarget();
				writeTexture.FinaliseTarget();
				
				bitmapTextPreRendered.Init(renderInterface, writeTexture);
				bitmapTextPreRendered.SetPosition(-400, -400);
			}
			
			auto renderFrame = [&](bool writingToScreenGrabTexture)
			{
				beModel* modelToRender = nullptr;
				switch (modelToUse)
				{
					case 0: modelToRender = nullptr; break;
					case 1: modelToRender = &model1; break;
					case 2: modelToRender = &model2; break;
					case 3: modelToRender = &model3; break;
					case 4: modelToRender = &model4; break;
					case 5: modelToRender = &model5; break;
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

				debugWorld->SetRenderAxes(renderAxes);
				debugWorld->Render(renderInterface, &colourShader, camera.GetViewMatrix(), camera.GetPosition());

				renderInterface->DisableZBuffer();
				textureShader2d.SetShaderParameters(renderInterface, camera.GetViewMatrix());
			
				if (bitmapScreenGrab.GetTexture() && !writingToScreenGrabTexture)
				{
					bitmapScreenGrab.Render(renderInterface);
					textureShader2d.Render(renderInterface, bitmapScreenGrab.GetIndexCount(), bitmapScreenGrab.GetTexture());
				}
			
				bitmapTexQuad.Render(renderInterface);
				textureShader2d.Render(renderInterface, bitmapTexQuad.GetIndexCount(), bitmapTexQuad.GetTexture());
	
				renderInterface->EnableAlpha();
				beStringBuilder sb;
				sb << "Dynamic Text\nMouseX:"<<mouse.GetX()<<"\nMouseY:"<<mouse.GetY();
				bitmapTextDynamic.InitText(renderInterface, &font, sb, 512.f, 0);
				bitmapTextDynamic.Render(renderInterface);
				textureShader2d.Render(renderInterface, bitmapTextDynamic.GetIndexCount(), bitmapTextDynamic.GetTexture(), beShaderTexture2d::TextureMode::Clamped);
			
				//bitmapTextPreRendered.Render(renderInterface);
				//textureShader2d.Render(renderInterface, bitmapTextPreRendered.GetIndexCount(), bitmapTextPreRendered.GetTexture());
				renderInterface->DisableAlpha();
			
				renderInterface->EnableZBuffer();
			};
			
			bool doScreenGrab = keyboard.IsPressed(beKeyboard::Button::Space);
			#ifdef ENABLE_RENDERDOC
			bool singleCapture = keyboard.IsPressed(beKeyboard::Button::F11) || doScreenGrab;// || keyboard.IsDown(beKeyboard::Button::Space);
			if (keyboard.IsPressed(beKeyboard::Button::F1)) { renderDoc.ToggleOverlay(); }
			if (singleCapture)
			{
				renderDoc.StartFrameCapture();
			}
			#endif
			
			renderInterface->BeginFrame();
			
			if (doScreenGrab)
			{
				screenGrabTexture.SetAsTarget(renderInterface);
				screenGrabTexture.Clear(renderInterface, Vec4(0.f, 0.f, 0.f, 0.f));
				renderFrame(true);
				renderInterface->RestoreRenderTarget();
				bitmapScreenGrab.Init(renderInterface, screenGrabTexture);
			}
			renderFrame(false);

			renderInterface->EndFrame();
			
			#ifdef ENABLE_RENDERDOC
			if (singleCapture)
			{
				renderDoc.EndFrameCapture();
			}
			#endif
			//bePRINTF("timeSinceStart %3.3f, dt:%3.3f", (float)beClock::GetSecondsSinceStart(), dt.ToSeconds());

		}
	}

	// return this part of the WM_QUIT message to Windows
	return win32Callback.retCode;
}

