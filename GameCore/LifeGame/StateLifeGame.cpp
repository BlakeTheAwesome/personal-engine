#include "BlakesEngine/bePCH.h"
#include "StateLifeGame.h"

#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/bekeyboard.h"
#include "BlakesEngine/Input/beMouse.h"
#include "BlakesEngine/Framework/beStateMachine.h"
#include "BlakesEngine/Framework/beAppData.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Math/beRandom.h"

#include <iomanip>

void StateLifeGame::Enter(beStateMachine* stateMachine)
{
	//auto renderInterface = m_appData->renderInterface;
	//m_font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));
	//m_textureShader2d.Init(renderInterface, beWString(L"Texture_p.cso"), beWString(L"Texture2d_v.cso"));
	
	m_camera.AttachGamepad(m_appData->gamepad);
	m_camera.AttachMouse(m_appData->mouse);
	
	m_cells.Initialise(m_appData);
}

void StateLifeGame::Exit(beStateMachine* stateMachine)
{
	m_camera.DetachGamepad();
	m_textureShader2d.Deinit();
	m_bitmapTextDynamic.Deinit();
	m_font.Deinit();
}

void StateLifeGame::Update(beStateMachine* stateMachine, float dt)
{
	auto gamepad = m_appData->gamepad;
	auto keyboard = m_appData->keyboard;
	//auto mouse = m_appData->mouse;
	//auto renderInterface = m_appData->renderInterface;

	if (gamepad->GetButtonReleased(beGamepad::B) || keyboard->IsPressed(beKeyboard::Button::Escape))
	{
		stateMachine->ChangeState(nullptr);
		return;
	}
	if (keyboard->IsPressed(beKeyboard::Button::P))
	{
		m_paused = !m_paused;
		m_timeUntilNextUpdate = 0.f;
	}

	m_camera.Update(dt);

	bool autoUpdate = !m_paused && ((m_timeUntilNextUpdate -= dt) < 0.f);
	bool step = autoUpdate || keyboard->IsPressed(beKeyboard::Button::Space);
	if (step)
	{
		m_timeUntilNextUpdate = m_updateTimeFrequency - m_timeUntilNextUpdate;
		m_cells.TickGame();
	}
}

void StateLifeGame::Render()
{
	auto renderInterface = m_appData->renderInterface;
	m_cells.Render(renderInterface);
	//auto keyboard = m_appData->keyboard;
	//auto mouse = m_appData->mouse;

	//if (!m_haveWrittenToTexture)
	
	/*
	{
		beTexture writeTexture;
		writeTexture.InitAsTarget(renderInterface, 512, 512);
		defer(
			writeTexture.Deinit();
		);
		//m_haveWrittenToTexture = true;
		writeTexture.SetAsTarget(renderInterface);
		writeTexture.Clear(renderInterface, Vec4(0.f, 0.f, 0.f, 0.0f));

		renderInterface->DisableZBuffer();
		m_textureShader2d.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
			
		//m_bitmapTexQuad.Render(renderInterface);
		//textureShader2d.Render(renderInterface, m_bitmapTexQuad.GetIndexCount(), m_bitmapTexQuad.GetTexture());

		

		beStringBuilder testString;
		for (int y = 0; y < m_cells.Length(); y++)
		{
			for (int x = 0; x < m_cells.Length(); x++)
			{
				char c = m_cells.At(x,y) ? 'o' : 'x';
				testString << c;
			}
			testString << "\n";
		}

		renderInterface->EnableAlpha();
		m_bitmapTextDynamic.InitText(renderInterface, &m_font, testString.ToString().c_str(), 512.f, 0, true);
		m_bitmapTextDynamic.SetPosition((float)(-writeTexture.GetWidth() / 2), (float)(writeTexture.GetHeight() / 2));
		m_bitmapTextDynamic.Render(renderInterface);
		m_textureShader2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture());
		renderInterface->DisableAlpha();
		renderInterface->EnableZBuffer();
		renderInterface->RestoreRenderTarget();
		writeTexture.FinaliseTarget();
		
		m_bitmapTextDynamic.Render(renderInterface);
		m_textureShader2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), beShaderTexture2d::TextureMode::Clamped);
	}*/
}


