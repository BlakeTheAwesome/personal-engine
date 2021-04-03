
#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beMacros.h"

#include "BlakesEngine/Platform/beWindows.h"
#include "BlakesEngine/External/RenderDoc-Manager/RenderDocManager.h"

#include "BlakesEngine/External/Misc/StreamToDebugOutput.h"

#include <shellapi.h>
#include <chrono>

import beShaderPack;
import beWindow;
import beMouse;
import beSystemEventManager;
import beStateMachine;
import beEnvironment;
import beGamepad;
import beKeyboard;
import beFrameTimer;
import beClock;
import beRenderInterface;
import StateLifeGame;
import StateRenderTest;
import StateWarGame;
import beAppData;
import beDebugWorld;

#ifdef DEBUG
#define ENABLE_RENDERDOC
#define RENDERDOC_PATH L"d:/Dev/Renderdoc/renderDoc.dll"
#define RENDERDOC_CAPTURE_PATH "d:/temp/renderDoc/capture"
#endif

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   PSTR lpCmdLine,
                   int nCmdShow)
{
	Cout2VisualStudioDebugOutput c2v;
	beEnvironment environment;
	if (!environment.InitialiseWithFile("cmdargs.txt"))
	{
		environment.Initialise(GetCommandLineA());
	}

	auto systemEventManager = std::make_unique<beSystemEventManager>();

	//beTypeTests::RunTypeTests();
	beClock::Initialise();
	
	beFrameTimer frameTimer;
	frameTimer.LimitFPS(60);
	
	beString windowName("TestWindow");
	auto window = new beWindow(systemEventManager.get(), hInstance, windowName, 1600, 900, false);
	
	std::unique_ptr<RenderDocManager> renderDoc;
	#ifdef ENABLE_RENDERDOC
	const bool usingRenderDoc = environment.Get("r");
	if (usingRenderDoc)
	{
		renderDoc = std::make_unique<RenderDocManager>(window->GetHWnd(), RENDERDOC_PATH, RENDERDOC_CAPTURE_PATH);
	}
	#endif

	auto renderInterface = std::make_unique<beRenderInterface>();
	renderInterface->Init(window, 0.01f, 1000.00f, true);
	defer( renderInterface->Deinit(); );

	beKeyboard keyboard;
	keyboard.Init(systemEventManager.get());
	beMouse mouse;
	mouse.Init(systemEventManager.get(), window);
	beGamepad gamepad;
	gamepad.Init(0);
	defer(
		gamepad.Deinit();
		mouse.Deinit();
		keyboard.Deinit();
	);

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
	} winProcCallback {window->GetHWnd()};

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
	
	
	beShaderPack shaderPack;
	shaderPack.shaderColour.Init(renderInterface.get(), beWString(L"Colour_p.cso"), beWString(L"Colour_v.cso"));
	shaderPack.shaderTexture.Init(renderInterface.get(), beWString(L"Texture_p.cso"), beWString(L"Texture_v.cso"));
	shaderPack.shaderTexture2d.Init(renderInterface.get(), beWString(L"Texture_p.cso"), beWString(L"Texture2dNorm_v.cso"), beWString(L"Texture2dPixels_v.cso"));
	shaderPack.shaderLitTexture.Init(renderInterface.get(), beWString(L"Light_p.cso"), beWString(L"Light_v.cso"));
	shaderPack.Init(renderInterface.get());
	defer
	(
		shaderPack.Deinit();
		shaderPack.shaderColour.Deinit();
		shaderPack.shaderTexture.Deinit();
		shaderPack.shaderTexture2d.Deinit();
		shaderPack.shaderLitTexture.Deinit();
	);


	auto debugWorld = std::make_unique<beDebugWorld>();
	debugWorld->Init(renderInterface.get());
	defer(debugWorld->Deinit(););

	beAppData appData;
	appData.environment = &environment;
	appData.systemEventManager = systemEventManager.get();
	appData.renderInterface = renderInterface.get();
	appData.debugWorld = debugWorld.get();
	appData.renderDocManager = renderDoc.get();
	appData.keyboard = &keyboard;
	appData.mouse = &mouse;
	appData.gamepad = &gamepad;
	appData.shaderPack = &shaderPack;
	appData.window = window;

	std::shared_ptr<beState> initialState;
	if (const beString* mode = environment.Get("mode"))
	{
		if (*mode == "test")
		{
			initialState = std::make_shared<StateRenderTest>(&appData);
		}
		else if (*mode == "life")
		{
			initialState = std::make_shared<StateLifeGame>(&appData);
		}
		else if (*mode == "war")
		{
			initialState = std::make_shared<StateWarGame>(&appData);
		}
	}
	else
	{
	}

	beStateMachine appStateMachine(move(initialState));

	// ACTUAL MAIN LOOP

	while(!win32Callback.quit)
	{
		systemEventManager->Update();

		beFrameTimer::Duration dt;
		const bool doStuff = frameTimer.StepFrame(&dt);

		if (doStuff)
		{
			using tFloatTime = std::chrono::duration<float>;
			const float fdt = std::chrono::duration_cast<tFloatTime>(dt).count();
			keyboard.Update(fdt);
			mouse.Update(fdt);
			gamepad.Update(fdt);
			
			if (!appStateMachine.Update(fdt))
			{
				win32Callback.quit = true;
				break;
			}
			renderInterface->Update(fdt);

			
			#ifdef ENABLE_RENDERDOC
			bool singleCapture = false;
			if (usingRenderDoc)
			{
				singleCapture = keyboard.IsPressed(beKeyboard::Button::F11) || keyboard.IsPressed(beKeyboard::Button::Space);// || keyboard.IsDown(beKeyboard::Button::Space);
				if (keyboard.IsPressed(beKeyboard::Button::F1)) { renderDoc->ToggleOverlay(); }
				if (singleCapture)
				{
					renderDoc->StartFrameCapture();
				}
			}
			#endif
			
			renderInterface->BeginFrame();
			appStateMachine.Render();
			renderInterface->EndFrame();
			
			#ifdef ENABLE_RENDERDOC
			if (usingRenderDoc && singleCapture)
			{
				renderDoc->EndFrameCapture();
			}
			#endif
			//LOG("timeSinceStart %3.3f, dt:%3.3f", (float)beClock::GetSecondsSinceStart(), dt.ToSeconds());

		}
	}

	// return this part of the WM_QUIT message to Windows
	return win32Callback.retCode;
}

