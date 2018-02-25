#include "BlakesEngine/bePCH.h"
#include "StateLifeGame.h"

#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/beKeyboard.h"
#include "BlakesEngine/Input/beMouse.h"
#include "BlakesEngine/Framework/beStateMachine.h"
#include "BlakesEngine/Framework/beAppData.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Rendering/beDebugWorld.h"
#include "BlakesEngine/Shaders/beShaderPack.h"
#include "BlakesEngine/Math/beRandom.h"
#include "BlakesEngine/Camera/beCameraUtils.h"
#include "BlakesEngine/Window/beWindow.h"

#include <iomanip>

void StateLifeGame::Enter(beStateMachine* stateMachine)
{
	//auto renderInterface = m_appData->renderInterface;
	//m_font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));
	//m_textureShader2d.Init(renderInterface, beWString(L"Texture_p.cso"), beWString(L"Texture2d_v.cso"));
	
	m_camera.AttachGamepad(m_appData->gamepad);
	m_camera.AttachMouse(m_appData->mouse);
	m_camera.SetPosition(Vec3(30.f, 30.f, -30.f));
	m_camera.SetYaw(0.5f);
	
	m_cells.Initialise(m_appData);
}

void StateLifeGame::Exit(beStateMachine* stateMachine)
{
	m_camera.DetachGamepad();
	m_bitmapTextDynamic.Deinit();
	m_font.Deinit();
}

void StateLifeGame::Update(beStateMachine* stateMachine, float dt)
{
	auto gamepad = m_appData->gamepad;
	auto keyboard = m_appData->keyboard;
	auto mouse = m_appData->mouse;
	auto renderInterface = m_appData->renderInterface;

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
	if (keyboard->IsPressed(beKeyboard::Button::T))
	{
		m_cells.ToggleRenderText();
	}

	m_camera.Update(dt);

	bool autoUpdate = !m_paused && ((m_timeUntilNextUpdate -= dt) < 0.f);
	bool step = autoUpdate || keyboard->IsPressed(beKeyboard::Button::Space);
	if (step)
	{
		m_timeUntilNextUpdate = m_updateTimeFrequency - m_timeUntilNextUpdate;
		m_cells.TickGame();
	}
	m_cells.Update(m_appData, dt);




	if (mouse->IsPressed(beMouse::LeftButton))
	{
		//auto window = m_appData->window;
		float screenX = (float)mouse->GetX();// -window->GetX();
		float screenY = (float)mouse->GetY();// - window->GetY();
		Vec2 screenDimensions = renderInterface->GetScreenSize();
		// screen dimensions not accounting for menu bars?
		float screenW = screenDimensions.x; //m_appData->window->GetWidth();
		float screenH = screenDimensions.y; //m_appData->window->GetHeight();
		Vec3 worldPos, worldDir;
		bool isInBounds = beCameraUtils::GetScreeenToWorldRay(*renderInterface, m_camera.GetViewMatrix(), screenX, screenY, screenW, screenH, &worldPos, &worldDir);
		if (isInBounds)
		{
			bePRINTF("MOUSE CLICK: (%.2f, %.2f) POS:{%3.3f, %3.3f, %3.3f} dir:{%3.3f, %3.3f, %3.3f}\r\n", screenX, screenY, worldPos.x, worldPos.y, worldPos.z, worldDir.x, worldDir.y, worldDir.z);
		}
	}

}

void StateLifeGame::Render()
{
	auto renderInterface = m_appData->renderInterface;
	m_cells.Render(renderInterface, m_appData->shaderPack, m_camera.GetViewMatrix(), m_camera.GetPosition());

	auto debugWorld = m_appData->debugWorld;
	debugWorld->SetRenderAxes(true);
	debugWorld->Render(renderInterface, &m_appData->shaderPack->shaderColour, m_camera.GetViewMatrix(), m_camera.GetPosition());
}


