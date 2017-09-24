#include "BlakesEngine/bePCH.h"
#include "LifeGameCells.h"

#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/bekeyboard.h"
#include "BlakesEngine/Input/beMouse.h"
#include "BlakesEngine/Framework/beStateMachine.h"
#include "BlakesEngine/Framework/beAppData.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Shaders/beShaderPack.h"
#include "BlakesEngine/Math/beRandom.h"

#include <iomanip>

void LifeGameCells::Render(beRenderInterface* renderInterface, beShaderPack* shaderPack, const Matrix& viewMatrix, const Vec3& cameraPosition)
{
	shaderPack->shaderColour.SetShaderParameters(renderInterface, viewMatrix);

	ID3D11DeviceContext* deviceContext = renderInterface->GetDeviceContext();
	unsigned int stride = m_vertexBuffer.ElementSize();
	unsigned int offset = 0;

	ID3D11Buffer* vertexBuffers[] ={m_vertexBuffer.GetBuffer()};
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shaderPack->shaderColour.Render(renderInterface, m_indexBuffer.NumElements(), 0);



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
		shaderPack->shaderTexture2d.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
			
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
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture());
		renderInterface->DisableAlpha();
		renderInterface->EnableZBuffer();
		renderInterface->RestoreRenderTarget();
		writeTexture.FinaliseTarget();
		
		m_bitmapTextDynamic.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), beShaderTexture2d::TextureMode::Clamped);
	}
}

void LifeGameCells::UpdateBlock(LifeGameCells::Block* block, float xPos, float yPos, float distance)
{
	float currentHeight = block->verts[2].position.y;
	float newHeight = beMath::Clamp(currentHeight+distance, 0.f, BlockHeight);


	//-x
	int faceOffset = 0;
	block->verts[faceOffset+2].position.y = newHeight;
	block->verts[faceOffset+3].position.y = newHeight;

	//+x
	faceOffset += 4;
	block->verts[faceOffset+2].position.y = newHeight;
	block->verts[faceOffset+3].position.y = newHeight;

	//-y
	faceOffset += 4;
	block->verts[faceOffset+2].position.y = newHeight;
	block->verts[faceOffset+3].position.y = newHeight;

	//+y
	faceOffset += 4;
	block->verts[faceOffset+2].position.y = newHeight;
	block->verts[faceOffset+3].position.y = newHeight;

	//+z
	faceOffset += 4;
	block->verts[faceOffset+0].position.y = newHeight;
	block->verts[faceOffset+1].position.y = newHeight;
	block->verts[faceOffset+2].position.y = newHeight;
	block->verts[faceOffset+3].position.y = newHeight;
}

void LifeGameCells::InitBlock(LifeGameCells::Block* block, float xPos, float yPos, float height)
{
	//-x
	int faceOffset = 0;
	//block->verts[faceOffset+0].normal = V3NX();
	//block->verts[faceOffset+1].normal = V3NX();
	//block->verts[faceOffset+2].normal = V3NX();
	//block->verts[faceOffset+3].normal = V3NX();
	block->verts[faceOffset+0].colour = V4NXW();
	block->verts[faceOffset+1].colour = V4NXW();
	block->verts[faceOffset+2].colour = V4NXW();
	block->verts[faceOffset+3].colour = V4NXW();
	block->verts[faceOffset+0].position = Vec4(xPos - HalfBlockLength, 0.f,    yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+1].position = Vec4(xPos - HalfBlockLength, 0.f,    yPos + HalfBlockLength, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos - HalfBlockLength, height, yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos - HalfBlockLength, height, yPos + HalfBlockLength, 1.f);

	//+x
	faceOffset += 4;
	//block->verts[faceOffset+0].normal = V3X();
	//block->verts[faceOffset+1].normal = V3X();
	//block->verts[faceOffset+2].normal = V3X();
	//block->verts[faceOffset+3].normal = V3X();
	block->verts[faceOffset+0].colour = V4XW();
	block->verts[faceOffset+1].colour = V4XW();
	block->verts[faceOffset+2].colour = V4XW();
	block->verts[faceOffset+3].colour = V4XW();
	block->verts[faceOffset+0].position = Vec4(xPos + HalfBlockLength, 0.f,    yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+1].position = Vec4(xPos + HalfBlockLength, 0.f,    yPos + HalfBlockLength, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos + HalfBlockLength, height, yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos + HalfBlockLength, height, yPos + HalfBlockLength, 1.f);

	//-y
	faceOffset += 4;
	//block->verts[faceOffset+0].normal = V3NY();
	//block->verts[faceOffset+1].normal = V3NY();
	//block->verts[faceOffset+2].normal = V3NY();
	//block->verts[faceOffset+3].normal = V3NY();
	block->verts[faceOffset+0].colour = V4NYW();
	block->verts[faceOffset+1].colour = V4NYW();
	block->verts[faceOffset+2].colour = V4NYW();
	block->verts[faceOffset+3].colour = V4NYW();
	block->verts[faceOffset+0].position = Vec4(xPos - HalfBlockLength, 0.f,    yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+1].position = Vec4(xPos + HalfBlockLength, 0.f,    yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos - HalfBlockLength, height, yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos + HalfBlockLength, height, yPos - HalfBlockLength, 1.f);

	//+y
	faceOffset += 4;
	//block->verts[faceOffset+0].normal = V3Y();
	//block->verts[faceOffset+1].normal = V3Y();
	//block->verts[faceOffset+2].normal = V3Y();
	//block->verts[faceOffset+3].normal = V3Y();
	block->verts[faceOffset+0].colour = V4YW();
	block->verts[faceOffset+1].colour = V4YW();
	block->verts[faceOffset+2].colour = V4YW();
	block->verts[faceOffset+3].colour = V4YW();
	block->verts[faceOffset+0].position = Vec4(xPos - HalfBlockLength, 0.f,    yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+1].position = Vec4(xPos + HalfBlockLength, 0.f,    yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos - HalfBlockLength, height, yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos + HalfBlockLength, height, yPos - HalfBlockLength, 1.f);

	//+z
	faceOffset += 4;
	//block->verts[faceOffset+0].normal = V3Z();
	//block->verts[faceOffset+1].normal = V3Z();
	//block->verts[faceOffset+2].normal = V3Z();
	//block->verts[faceOffset+3].normal = V3Z();
	block->verts[faceOffset+0].colour = V4ZW();
	block->verts[faceOffset+1].colour = V4ZW();
	block->verts[faceOffset+2].colour = V4ZW();
	block->verts[faceOffset+3].colour = V4ZW();
	block->verts[faceOffset+0].position = Vec4(xPos - HalfBlockLength, height, yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+1].position = Vec4(xPos + HalfBlockLength, height, yPos - HalfBlockLength, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos - HalfBlockLength, height, yPos + HalfBlockLength, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos + HalfBlockLength, height, yPos + HalfBlockLength, 1.f);

}

void LifeGameCells::Initialise(beAppData* appData)
{
	auto renderInterface = appData->renderInterface;

	m_font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));

	beRandom rng;
	rng.InitFromSystemTime();
	int numBlocks = m_renderBlocks.Capacity();
	m_renderBlocks.SetCountUninitialised(numBlocks);

	for (int y : RangeIter(m_cells.Length()))
	{
		for (int x : RangeIter(m_cells.Length()))
		{
			int arrayIndex = m_cells.ToIndex(x, y);

			bool live = rng.NextBool();
			m_cells[arrayIndex] = live;

			float xPos = x*BlockLength;
			float yPos = y*BlockLength;
			float height = live ? BlockHeight : 0.f;
			bePRINTF("Writing to block[%d]", arrayIndex);
			InitBlock(&m_renderBlocks.At(arrayIndex), xPos, yPos, height);
		}
	}

	int numVerts = numBlocks * Block::VertsPerBlock;
	//int numTris = numBlocks * Block::TrisPerBlock;
	bool success = m_vertexBuffer.Allocate(renderInterface, sizeof(GridVertexFormat), numVerts, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, m_renderBlocks.begin());
	BE_ASSERT(success);

	u32 FaceIndexLayout[Block::IndicesPerFace] = {
		0, 1, 2, 2, 1, 3,
	};

	beVector<u32> indices(numBlocks * Block::IndicesPerBlock);
	indices.SetCount(numBlocks * Block::IndicesPerBlock);
	for (int blockIndex : RangeIter(m_renderBlocks.Count()))
	{
		int vertBlockStart = blockIndex * Block::VertsPerBlock;
		int indexBlockStart = blockIndex * Block::IndicesPerBlock;

		for (int faceIndex : RangeIter(Block::FacesPerBlock))
		{
			int vertFaceStart = vertBlockStart + (faceIndex * Block::VertsPerFace);
			int indexFaceStart = indexBlockStart + (faceIndex * Block::IndicesPerFace);

			for (int indexIndex : RangeIter(Block::IndicesPerFace))
			{
				indices[indexFaceStart + indexIndex] = vertFaceStart + FaceIndexLayout[indexIndex];
			}
		}
	}

	success = m_indexBuffer.Allocate(renderInterface, decltype(indices)::element_size, indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, indices.begin());
	BE_ASSERT(success);
}

void LifeGameCells::Finalise()
{
	m_indexBuffer.Release();
	m_vertexBuffer.Release();
	m_bitmapTextDynamic.Deinit();
	m_font.Deinit();
}

void LifeGameCells::Update(beAppData* appData, float dt)
{
	auto renderInterface = appData->renderInterface;

	float distance = m_animationDistancePerSecond * dt;
	float negDist = -distance;

	for (auto iter : m_cells.GridIter())
	{
		Block* block = &m_renderBlocks[iter.index];
		float increment = *iter ? distance : negDist;
		float xPos = iter.xPos() * BlockLength;
		float yPos = iter.yPos() * BlockLength;

		UpdateBlock(block, xPos, yPos, increment);
	}

	{
		void* bufferMem = m_vertexBuffer.Map(renderInterface);
		BE_ASSERT(m_vertexBuffer.BufferSize() == m_renderBlocks.DataSize());
		BE_MEMCPY(bufferMem, m_renderBlocks.begin(), m_renderBlocks.DataSize());
		m_vertexBuffer.Unmap(renderInterface);
	}
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
				bool live = adjecentLivingCells == 3;
				int index = m_nextCells.ToIndex(x, y);
				m_nextCells[index] = live;
			}
		}
	}
	
	m_cells = m_nextCells;
}


