#pragma once
#include "BlakesEngine/Rendering/beBitmap.h"
#include "BlakesEngine/Rendering/beFont.h"
#include "BlakesEngine/Shaders/beShaderTexture2d.h"
#include "BlakesEngine/DataStructures/beFastGrid.h"
#include "WarGameCells.h"

import beStateMachine;
import beFlightCamera;

struct beAppData;
class beDebugWorld;

class StateWarGame : public beState
{
	public:
	StateWarGame() = delete;
	StateWarGame(beAppData* appData) : m_appData(appData) {}

	void Enter(beStateMachine* stateMachine) override;
	void Exit(beStateMachine* stateMachine) override;
	void Update(beStateMachine* stateMachine, float dt) override;
	void Render() override;

	private:

	beAppData* m_appData;
	beFont m_font;
	beBitmap m_bitmapTextDynamic;
	beFlightCamera m_camera;

	WarGameCells m_cells;
	float m_timeUntilNextUpdate = 0.f;
	float m_updateTimeFrequency = 0.5f;
	bool m_paused = false;
};
