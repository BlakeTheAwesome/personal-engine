#pragma once
#include "BlakesEngine/framework/beState.h"
#include "BlakesEngine/Rendering/beBitmap.h"
#include "BlakesEngine/Rendering/beFont.h"
#include "BlakesEngine/Shaders/beShaderTexture2d.h"
#include "BlakesEngine/DataStructures/beFastGrid.h"
#include "BlakesEngine/Camera/beFlightCamera.h"
#include "LifeGameCells.h"

struct beAppData;
class beDebugWorld;

class StateLifeGame : public beState
{
	public:
	StateLifeGame() = delete;
	StateLifeGame(beAppData* appData) : m_appData(appData) {}

	void Enter(beStateMachine* stateMachine) override;
	void Exit(beStateMachine* stateMachine) override;
	void Update(beStateMachine* stateMachine, float dt) override;
	void Render() override;

	private:

	beAppData* m_appData;
	beFont m_font;
	beBitmap m_bitmapTextDynamic;
	beShaderTexture2d m_textureShader2d;
	beFlightCamera m_camera;

	LifeGameCells m_cells;
	float m_timeUntilNextUpdate = 0.f;
	float m_updateTimeFrequency = 0.5f;
	bool m_paused = false;
};
