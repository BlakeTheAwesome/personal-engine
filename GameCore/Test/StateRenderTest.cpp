
#include "BlakesEngine/bePCH.h"
#include "StateRenderTest.h"

#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beTypeTests.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/bekeyboard.h"
#include "BlakesEngine/Input/beMouse.h"
#include "BlakesEngine/Window/beWindow.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Rendering/beDebugWorld.h"
#include "BlakesEngine/Shaders/beShaderPack.h"
#include "BlakesEngine/External/RenderDoc-Manager/RenderDocManager.h"
#include "BlakesEngine/Framework/beStateMachine.h"
#include "BlakesEngine/Framework/beAppData.h"

#include "BlakesEngine/External/Misc/StreamToDebugOutput.h"
#include <shellapi.h>

#ifdef DEBUG
#define ENABLE_RENDERDOC
#define RENDERDOC_PATH L"d:/Dev/Renderdoc/renderDoc.dll"
#define RENDERDOC_CAPTURE_PATH "d:/temp/renderDoc/capture"
#endif

void StateRenderTest::Enter(beStateMachine* stateMachine)
{
	auto renderInterface = m_appData->renderInterface;

	m_debugWorld = PIMPL_NEW(beDebugWorld)();
	m_debugWorld->Init(renderInterface);
	
	m_font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));
	m_model1.Init(renderInterface, beWString(L"boar.dds"));
	m_model2.InitWithFilename(renderInterface, "cube.obj", beWString(L"seafloor.dds"));
	m_model3.InitWithFilename(renderInterface, "cube2.obj", beWString(L"seafloor.dds"));
	m_model4.InitWithFilename(renderInterface, "teapot.obj", beWString(L"seafloor.dds"));
	m_model5.InitWithFilename(renderInterface, "boxes.obj", beWString(L"barrels.dds"));

	m_screenGrabTexture.InitAsTarget(renderInterface, 256, 256);

	m_bitmapTexQuad.Init(renderInterface, 128, 128, beWString(L"boar.dds"));
	m_bitmapTexQuad.SetPosition(1024/2-128, 768/2-128);
	m_bitmapTextDynamic.SetColour(Vec4(0.f, 1.f, 0.8f, 1.f));
	
	m_camera.AttachGamepad(m_appData->gamepad);
	m_camera.AttachMouse(m_appData->mouse);
	
}

void StateRenderTest::Exit(beStateMachine* stateMachine)
{
	m_camera.DetachGamepad();

	m_bitmapScreenGrab.Deinit();
	m_bitmapTextPreRendered.Deinit();
	m_bitmapTextDynamic.Deinit();
	m_bitmapTexQuad.Deinit();

	m_screenGrabTexture.FinaliseTarget();
	m_screenGrabTexture.Deinit();

	m_model5.Deinit();
	m_model4.Deinit();
	m_model3.Deinit();
	m_model2.Deinit();
	m_model1.Deinit();
	m_font.Deinit();

	m_debugWorld->Deinit();
	PIMPL_DELETE(m_debugWorld);
}

void StateRenderTest::Update(beStateMachine* stateMachine, float dt)
{
	const int numShaders = 3;
	const int numModels = 6;
	
	auto gamepad = m_appData->gamepad;
	auto keyboard = m_appData->keyboard;
	auto mouse = m_appData->mouse;
	auto renderInterface = m_appData->renderInterface;

	if (gamepad->GetButtonReleased(beGamepad::A) || keyboard->IsPressed(beKeyboard::Button::W))
	{
		renderInterface->ToggleWireframe();
	}
	if (gamepad->GetButtonReleased(beGamepad::B) || keyboard->IsPressed(beKeyboard::Button::Escape))
	{
		stateMachine->ChangeState(nullptr);
		return;
	}
	if (gamepad->GetButtonReleased(beGamepad::Y))
	{
		m_shaderToUse++;
		m_shaderToUse %= numShaders;
	}
	if (gamepad->GetButtonReleased(beGamepad::X) || mouse->IsPressed(beMouse::Button::MiddleButton))
	{
		m_modelToUse++;
		m_modelToUse %= numModels;
	}
	if (gamepad->GetButtonReleased(beGamepad::Select))
	{
		m_renderAxes = !m_renderAxes;
	}
	if (gamepad->GetButtonReleased(beGamepad::Up))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x, bitmapPosition.y + 10.f);
	}
	if (gamepad->GetButtonReleased(beGamepad::Down))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x, bitmapPosition.y - 10.f);
	}
	if (gamepad->GetButtonReleased(beGamepad::Left))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x - 10.f, bitmapPosition.y);
	}
	if (gamepad->GetButtonReleased(beGamepad::Right))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x + 10.f, bitmapPosition.y);
	}

	m_camera.Update(dt);

}

void StateRenderTest::Render()
{
	auto renderInterface = m_appData->renderInterface;
	auto keyboard = m_appData->keyboard;
	auto mouse = m_appData->mouse;
	auto shaderPack = m_appData->shaderPack;

	if (!m_haveWrittenToTexture)
	{
		beTexture writeTexture;
		writeTexture.InitAsTarget(renderInterface, 512, 512);
		defer(
			writeTexture.Deinit();
		);
		m_haveWrittenToTexture = true;
		writeTexture.SetAsTarget(renderInterface);
		writeTexture.Clear(renderInterface, Vec4(0.f, 0.f, 0.f, 0.0f));
		m_model4.Render(renderInterface);
		shaderPack->shaderColour.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
		shaderPack->shaderColour.Render(renderInterface, m_model4.GetIndexCount(), 0);
				
		renderInterface->DisableZBuffer();
		shaderPack->shaderTexture2d.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
			
		//m_bitmapTexQuad.Render(renderInterface);
		//textureShader2d.Render(renderInterface, m_bitmapTexQuad.GetIndexCount(), m_bitmapTexQuad.GetTexture());
		
		renderInterface->EnableAlpha();
		m_bitmapTextDynamic.InitText(renderInterface, &m_font, "initial string", 640.f, 0, false);
		m_bitmapTextDynamic.SetPosition((float)(-writeTexture.GetWidth() / 2), (float)(writeTexture.GetHeight() / 2));
		m_bitmapTextDynamic.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture());
		renderInterface->DisableAlpha();
		renderInterface->EnableZBuffer();
		renderInterface->RestoreRenderTarget();
		writeTexture.FinaliseTarget();
				
		m_bitmapTextPreRendered.Init(renderInterface, writeTexture);
		m_bitmapTextPreRendered.SetPosition(-400, -400);
	}
			
	auto renderFrame = [&](bool writingToScreenGrabTexture)
	{
		beModel* modelToRender = nullptr;
		switch (m_modelToUse)
		{
			case 0: modelToRender = nullptr; break;
			case 1: modelToRender = &m_model1; break;
			case 2: modelToRender = &m_model2; break;
			case 3: modelToRender = &m_model3; break;
			case 4: modelToRender = &m_model4; break;
			case 5: modelToRender = &m_model5; break;
		}
			
		if (modelToRender)
		{
			modelToRender->Render(renderInterface);

			switch (m_shaderToUse)
			{
				case 0:
					shaderPack->shaderLitTexture.SetShaderParameters(renderInterface, m_camera.GetViewMatrix(), m_camera.GetPosition());
					shaderPack->shaderLitTexture.Render(renderInterface, modelToRender->GetIndexCount(), modelToRender->GetTexture());
				break;
				case 1:
					shaderPack->shaderTexture.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
					shaderPack->shaderTexture.Render(renderInterface, modelToRender->GetIndexCount(), modelToRender->GetTexture());
				break;
				case 2:
					shaderPack->shaderColour.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
					shaderPack->shaderColour.Render(renderInterface, modelToRender->GetIndexCount(), 0);
				break;
			}
		}

		m_debugWorld->SetRenderAxes(m_renderAxes);
		m_debugWorld->Render(renderInterface, &shaderPack->shaderColour, m_camera.GetViewMatrix(), m_camera.GetPosition());

		renderInterface->DisableZBuffer();
		shaderPack->shaderTexture2d.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
			
		if (m_bitmapScreenGrab.GetTexture() && !writingToScreenGrabTexture)
		{
			m_bitmapScreenGrab.Render(renderInterface);
			shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapScreenGrab.GetIndexCount(), m_bitmapScreenGrab.GetTexture());
		}
			
		m_bitmapTexQuad.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTexQuad.GetIndexCount(), m_bitmapTexQuad.GetTexture());
	
		renderInterface->EnableAlpha();
		beStringBuilder sb;
		sb << "Dynamic Text\nMouseX:"<<mouse->GetX()<<"\nMouseY:"<<mouse->GetY();
		m_bitmapTextDynamic.InitText(renderInterface, &m_font, sb, 512.f, 0, false);
		m_bitmapTextDynamic.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), beShaderTexture2d::TextureMode::Clamped);
			
		//bitmapTextPreRendered.Render(renderInterface);
		//textureShader2d.Render(renderInterface, bitmapTextPreRendered.GetIndexCount(), bitmapTextPreRendered.GetTexture());
		renderInterface->DisableAlpha();
			
		renderInterface->EnableZBuffer();
	};
	
	bool doScreenGrab = keyboard->IsPressed(beKeyboard::Button::Space);
	if (doScreenGrab)
	{
		m_screenGrabTexture.SetAsTarget(renderInterface);
		m_screenGrabTexture.Clear(renderInterface, Vec4(0.f, 0.f, 0.f, 0.f));
		renderFrame(true);
		renderInterface->RestoreRenderTarget();
		m_bitmapScreenGrab.Init(renderInterface, m_screenGrabTexture);
	}
	renderFrame(false);

}


