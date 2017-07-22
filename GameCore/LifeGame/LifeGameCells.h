#pragma once
#include "BlakesEngine/framework/beState.h"
#include "BlakesEngine/Rendering/beBitmap.h"
#include "BlakesEngine/Rendering/beFont.h"
#include "BlakesEngine/Shaders/beShaderTexture2d.h"
#include "BlakesEngine/DataStructures/beFastGrid.h"
#include "BlakesEngine/Camera/beFlightCamera.h"

struct beAppData;
class beDebugWorld;
class beRenderInterface;
class beApData;

struct LifeGameCells
{
	~LifeGameCells() { Finalise(); }
	void Initialise(beAppData* appData);
	void Finalise();
	void TickGame();
	void Render(beRenderInterface* ri);

	private:
	beFont m_font;
	beBitmap m_bitmapTextDynamic;
	beShaderTexture2d m_textureShader2d;
	beFlightCamera m_camera;

	beFastGrid<bool, 32> m_cells;
	decltype(m_cells) m_nextCells;
};
