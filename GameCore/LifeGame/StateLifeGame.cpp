module;
#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Rendering/beDebugWorld.h"

#include <iomanip>

module StateLifeGame;

import beShaderPack;
import beWindow;
import beRandom;
import beStateMachine;
import beCameraUtils;
import beGamepad;
import beKeyboard;
import beMouse;
import beRenderInterface;
import beAppData;

void StateLifeGame::Enter(beStateMachine* stateMachine)
{
	//auto renderInterface = m_appData->renderInterface;
	//m_font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));
	//m_textureShader2d.Init(renderInterface, beWString(L"Texture_p.cso"), beWString(L"Texture2d_v.cso"));
	
	m_camera.AttachGamepad(m_appData->gamepad);
	m_camera.AttachMouse(m_appData->mouse);
	m_camera.SetPosition(Vec3(30.f, -16.f, 30.f));
	m_camera.SetYaw(0.0f);
	m_camera.SetPitch(-0.8f);
	
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
	//auto mouse = m_appData->mouse;
	//auto renderInterface = m_appData->renderInterface;
	auto debugWorld = m_appData->debugWorld;

	if (gamepad->GetPressed(beGamepad::B) || keyboard->IsPressed(beKeyboard::Button::Escape))
	{
		stateMachine->ChangeState(nullptr);
		return;
	}
	if (keyboard->IsPressed(beKeyboard::Button::P) || gamepad->GetPressed(beGamepad::Button::A))
	{
		m_paused = !m_paused;
		m_timeUntilNextUpdate = 0.f;
	}
	if (keyboard->IsPressed(beKeyboard::Button::K))
	{
		m_cells.KillAll();
	}
	if (keyboard->IsPressed(beKeyboard::Button::Up) || gamepad->GetPressed(beGamepad::Button::Up))
	{
		m_updateTimeFrequency -= 0.1f;
		if (m_updateTimeFrequency < 0.f)
		{
			m_updateTimeFrequency = 0.f;
		}
		LOG("Up: m_updateTimeFrequency = {:.3f}", m_updateTimeFrequency);
	}
	if (keyboard->IsPressed(beKeyboard::Button::Down) || gamepad->GetPressed(beGamepad::Button::Down))
	{
		m_updateTimeFrequency += 0.1f;
		LOG("Down: m_updateTimeFrequency = {:.3f}", m_updateTimeFrequency);
	}
	if (keyboard->IsPressed(beKeyboard::Button::T))
	{
		m_cells.ToggleRenderText();
	}
	if (keyboard->IsPressed(beKeyboard::Button::M))
	{
		debugWorld->ToggleRenderMouseRay();
	}

	m_camera.Update(dt);
	debugWorld->Update(*m_appData, m_camera.GetViewMatrix());

	const bool autoUpdate = !m_paused && ((m_timeUntilNextUpdate -= dt) < 0.f);
	const bool step = autoUpdate || keyboard->IsPressed(beKeyboard::Button::Space) || gamepad->GetPressed(beGamepad::Button::X);
	if (step)
	{
		m_timeUntilNextUpdate = m_updateTimeFrequency - m_timeUntilNextUpdate;
		m_cells.TickGame();
	}
	m_cells.Update(m_appData, dt, m_camera.GetViewMatrix());




	//if (mouse->IsPressed(beMouse::LeftButton))
	//{
	//	//auto window = m_appData->window;
	//	float screenX = (float)mouse->GetX();// -window->GetX();
	//	float screenY = (float)mouse->GetY();// - window->GetY();
	//	Vec2 screenDimensions = renderInterface->GetScreenSize();
	//	// screen dimensions not accounting for menu bars?
	//	float screenW = screenDimensions.x; //m_appData->window->GetWidth();
	//	float screenH = screenDimensions.y; //m_appData->window->GetHeight();
	//	Vec3 worldPos, worldDir;
	//	bool isInBounds = beCameraUtils::GetScreeenToWorldRay(*renderInterface, m_camera.GetViewMatrix(), screenX, screenY, screenW, screenH, &worldPos, &worldDir);
	//	if (isInBounds)
	//	{
	//		LOG("MOUSE CLICK: ({:.2f}, {:.2f}) POS:({:.3f}, {:.3f}, {:.3f}) dir:({:.3f}, {:.3f}, {:.3f})\r\n", screenX, screenY, worldPos.x, worldPos.y, worldPos.z, worldDir.x, worldDir.y, worldDir.z);
	//		LOG("MOUSE Deets:");
	//		PositionFromMatrix(m_camera.GetViewMatrix());
	//	}
	//
	//	if (auto realWorldPos = beCameraUtils::WorldPosFromScreenPos(*renderInterface, m_camera.GetViewMatrix(), m_camera.GetPosition(), (int)screenX, (int)screenY))
	//	{
	//		
	//		LOG("Collision! mouse pos:({:.3f}, {:.3f}, {:.3f}) World collision at:({:.3f}, {:.3f}, {:.3f})\r\n", m_camera.GetPosition().x, m_camera.GetPosition().y, m_camera.GetPosition().z, realWorldPos->x, realWorldPos->y, realWorldPos->z);
	//	}
	//}

}

void StateLifeGame::Render()
{
	auto renderInterface = m_appData->renderInterface;
	auto shaderPack = m_appData->shaderPack;

	shaderPack->UpdateFrameBuffers(renderInterface, m_camera.GetViewMatrix());

	m_cells.Render(renderInterface, m_appData->shaderPack, m_camera.GetViewMatrix(), m_camera.GetPosition());

	auto debugWorld = m_appData->debugWorld;
	debugWorld->SetRenderAxes(true);
	debugWorld->Render(renderInterface, &m_appData->shaderPack->shaderColour, m_camera.GetViewMatrix(), m_camera.GetPosition());
}


