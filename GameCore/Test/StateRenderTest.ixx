module;
#include "BlakesEngine/Rendering/beDebugWorld.h"
export module StateRenderTest;

import beStateMachine;
import beFlightCamera;
import beModel;
import beShaderPack;
import beBitmap;
import beFont;
import beTexture;
import beRenderInterface;
import beAppData;

export class StateRenderTest : public beState
{
	public:
	StateRenderTest() = delete;
	StateRenderTest(beAppData* appData) : m_appData(appData) {}

	void Enter(beStateMachine* stateMachine) override;
	void Exit(beStateMachine* stateMachine) override;
	void Update(beStateMachine* stateMachine, float dt) override;
	void Render() override;

	private:
	void InitGrid(beRenderInterface* ri);

	beAppData* m_appData = nullptr;
	beFont m_font;
	beVector<beModel> m_models;
	beModel m_gridModel;
	beTexture m_screenGrabTexture;
	beBitmap m_bitmapTexQuadNorm;
	beBitmap m_bitmapTexQuadPixelCoord;
	beBitmap m_bitmapTextDynamic;
	beBitmap m_bitmapTextPreRendered;
	beBitmap m_bitmapScreenGrab;

	beFlightCamera m_camera;

	beRendering::ShaderType m_shaderToUse = beRendering::ShaderType::Default;
	int m_modelToUse = 3;

	[[maybe_unused]] bool m_haveWrittenToTexture = false;
	bool m_renderAxes = true;
	bool m_renderGrid = true;
	bool m_showGroundFilled = false;
};
