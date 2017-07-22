#include "BlakesEngine/bePCH.h"
#include "LifeGameCells.h"

#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/bekeyboard.h"
#include "BlakesEngine/Input/beMouse.h"
#include "BlakesEngine/Framework/beStateMachine.h"
#include "BlakesEngine/Framework/beAppData.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Math/beRandom.h"

#include <iomanip>

void LifeGameCells::Render(beRenderInterface* renderInterface)
{
	//if (!m_haveWrittenToTexture)
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
	}
}

void LifeGameCells::Initialise(beAppData* appData)
{
	auto renderInterface = appData->renderInterface;

	m_font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));
	m_textureShader2d.Init(renderInterface, beWString(L"Texture_p.cso"), beWString(L"Texture2d_v.cso"));

	beRandom rng;
	rng.InitFromSystemTime();
	for (int y = 0; y < m_cells.Length(); y++)
	{
		for (int x = 0; x < m_cells.Length(); x++)
		{
			m_cells.At(x, y) = rng.NextBool();
		}
	}
}

void LifeGameCells::Finalise()
{
	m_textureShader2d.Deinit();
	m_bitmapTextDynamic.Deinit();
	m_font.Deinit();
}

void LifeGameCells::TickGame()
{
	const int length = m_cells.Length();
	for (int y = 0; y < length; y++)
	{
		bool onTopEdge = y == 0;
		bool onBottomEdge = y == (length-1);
		for (int x = 0; x < length; x++)
		{
			bool onLeftEdge = x == 0;
			bool onRightEdge = x == (length-1);
			int adjecentLivingCells = 0;
			// left cell
			if (!onLeftEdge)
			{
				if (m_cells.At(x - 1, y))
				{
					adjecentLivingCells++;
				}
				// Left Top
				if (!onTopEdge)
				{
					if (m_cells.At(x - 1, y - 1))
					{
						adjecentLivingCells++;
					}
				}
				// Left Bottom
				if (!onBottomEdge)
				{
					if (m_cells.At(x - 1, y + 1))
					{
						adjecentLivingCells++;
					}
				}
			}
			// right cell
			if (!onRightEdge)
			{
				if (m_cells.At(x + 1, y))
				{
					adjecentLivingCells++;
				}
				// Right Top
				if (!onTopEdge)
				{
					if (m_cells.At(x + 1, y - 1))
					{
						adjecentLivingCells++;
					}
				}
				// Right Bottom
				if (!onBottomEdge)
				{
					if (m_cells.At(x + 1, y + 1))
					{
						adjecentLivingCells++;
					}
				}
			}
			// top cell
			if (!onTopEdge)
			{
				if (m_cells.At(x, y - 1))
				{
					adjecentLivingCells++;
				}
			}
			// bottom cell
			if (!onBottomEdge)
			{
				if (m_cells.At(x, y + 1))
				{
					adjecentLivingCells++;
				}
			}
			//Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
			//Any live cell with two or three live neighbours lives on to the next generation.
			//Any live cell with more than three live neighbours dies, as if by overpopulation.
			//Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
			if (adjecentLivingCells != 2)
			{
				m_nextCells.At(x, y) = adjecentLivingCells == 3;
			}
		}
	}
	
	m_cells = m_nextCells;
}


