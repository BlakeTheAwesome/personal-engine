#pragma once
#include "BlakesEngine/Framework/beState.h"

#include "BlakesEngine/Rendering/beBitmap.h"
#include "BlakesEngine/Rendering/beModel.h"
#include "BlakesEngine/Rendering/beTexture.h"
#include "BlakesEngine/Rendering/beFont.h"
#include "BlakesEngine/Shaders/beShaderColour.h"
#include "BlakesEngine/Shaders/beShaderTexture.h"
#include "BlakesEngine/Shaders/beShaderTexture2d.h"
#include "BlakesEngine/Shaders/beShaderLitTexture.h"
#include "BlakesEngine/Camera/beFlightCamera.h"

struct beAppData;
class beDebugWorld;

class StateRenderTest : public beState
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

	beAppData* m_appData;
	beFont m_font;
	beModel m_model1;
	beModel m_model2;
	beModel m_model3;
	beModel m_model4;
	beModel m_model5;
	beModel m_gridModel;
	beRenderBuffer m_gridModelLinesIndexBuffer;
	beTexture m_screenGrabTexture;
	beBitmap m_bitmapTexQuad;
	beBitmap m_bitmapTextDynamic;
	beBitmap m_bitmapTextPreRendered;
	beBitmap m_bitmapScreenGrab;

	beFlightCamera m_camera;

	int m_shaderToUse = 0;
	int m_modelToUse = 3;

	bool m_haveWrittenToTexture = false;
	bool m_renderAxes = true;
	bool m_renderGrid = true;
};
