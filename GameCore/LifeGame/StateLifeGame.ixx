export module StateLifeGame;

import LifeGameCells;
import beStateMachine;
import beFlightCamera;
import beShaderPack;
import beBitmap;
import beFont;
import beFastGrid;
import beAppData;


export class StateLifeGame : public beState
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
	beFlightCamera m_camera;

	LifeGameCells m_cells;
	float m_timeUntilNextUpdate = 0.f;
	float m_updateTimeFrequency = 0.5f;
	bool m_paused = false;
};
