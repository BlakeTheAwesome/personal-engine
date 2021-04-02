#include "BlakesEngine/bePCH.h"
#include "LifeGameCells.h"

#include "BlakesEngine/Core/beContainerHelpers.h"
#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/beKeyboard.h"
#include "BlakesEngine/Input/beMouse.h"
#include "BlakesEngine/Framework/beStateMachine.h"
#include "BlakesEngine/Framework/beAppData.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Shaders/beShaderPack.h"
#include "BlakesEngine/Math/beRandom.h"
#include "BlakesEngine/Camera/beCameraUtils.h"
#include "BlakesEngine/Math/beIntersection.h"

import RangeIter;
import beMath;

void LifeGameCells::Render(beRenderInterface* renderInterface, beShaderPack* shaderPack, const Matrix& viewMatrix, const Vec3& cameraPosition)
{
	shaderPack->shaderColour.SetShaderParameters(renderInterface);

	ID3D11DeviceContext* deviceContext = renderInterface->GetDeviceContext();
	const unsigned int stride = m_vertexBuffer.ElementSize();
	const unsigned int offset = 0;

	ID3D11Buffer* vertexBuffers[] ={m_vertexBuffer.GetBuffer()};
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_indexBuffer.D3DIndexTopology());
	shaderPack->shaderColour.SetActive(renderInterface);
	shaderPack->shaderColour.Render(renderInterface, m_indexBuffer.NumElements(), 0);

	if (m_renderTextCells)
	{
		beTexture writeTexture;
		beTexture::LoadOptions textureLoadOptions;
		textureLoadOptions.height = 512;
		textureLoadOptions.width = 512;
		textureLoadOptions.format = beTextureFormat::R32G32B32_FLOAT;
		writeTexture.InitAsTarget(renderInterface, textureLoadOptions);
		defer(
			writeTexture.Deinit();
		);
		writeTexture.SetAsTarget(renderInterface);
		writeTexture.Clear(renderInterface, Vec4(0.f, 0.f, 0.f, 0.0f));

		renderInterface->DisableZBuffer();
		shaderPack->shaderTexture2d.SetShaderParameters(renderInterface, viewMatrix);

		
		beStringBuilder testString;
		for (int y : RangeIterReverse(m_cells.Length()))
		{
			for (int x : RangeIter(m_cells.Length()))
			{
				const char c = m_cells.At(x, y) ? 'o' : 'x';
				testString << c;
			}
			testString << "\n";
		}

		renderInterface->EnableAlpha();
		m_bitmapTextDynamic.InitText(renderInterface, &m_font, testString.ToString().c_str(), 0.5f, 512.f, 0, true, beFont::WrapMode::NoWrap);
		m_bitmapTextDynamic.SetPosition(60.f, 0.f);
		m_bitmapTextDynamic.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), false);
		renderInterface->DisableAlpha();
		renderInterface->EnableZBuffer();
		renderInterface->RestoreRenderTarget();
		writeTexture.FinaliseTarget();

		m_bitmapTextDynamic.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), false, beShaderTexture2d::TextureMode::Clamped);
	}
}

void LifeGameCells::UpdateBlock(LifeGameCells::Block* block, float xPos, float yPos, float distance, bool highlight)
{
	const float currentHeight = block->verts[2].position.z;
	const float newHeight = Clamp(currentHeight+distance, 0.f, BlockHeight);


	//-x
	size_t faceOffset = 0;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+2].colour = V4XYW() * newHeight;
	block->verts[faceOffset+3].colour = V4XYW() * newHeight;

	//+x
	faceOffset += 4;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+2].colour = V4XW() * newHeight;
	block->verts[faceOffset+3].colour = V4XW() * newHeight;

	//-y
	faceOffset += 4;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+2].colour = V4YZW() * newHeight;
	block->verts[faceOffset+3].colour = V4YZW() * newHeight;

	//+y
	faceOffset += 4;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+2].colour = V4YW() * newHeight;
	block->verts[faceOffset+3].colour = V4YW() * newHeight;

	//+z
	faceOffset += 4;
	block->verts[faceOffset+0].position.z = newHeight;
	block->verts[faceOffset+1].position.z = newHeight;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+0].colour = V4ZW() * newHeight;
	block->verts[faceOffset+1].colour = V4ZW() * newHeight;
	block->verts[faceOffset+2].colour = V4ZW() * newHeight;
	block->verts[faceOffset+3].colour = V4ZW() * newHeight;

	if (highlight)
	{
		for (GridVertexFormat& vert : block->verts)
		{
			vert.colour = V41();
		}
	}
}

void LifeGameCells::InitBlock(LifeGameCells::Block* block, float xPos, float yPos, float height)
{
	//-x
	size_t faceOffset = 0;
	//block->verts[faceOffset+0].normal = V3NX();
	//block->verts[faceOffset+1].normal = V3NX();
	//block->verts[faceOffset+2].normal = V3NX();
	//block->verts[faceOffset+3].normal = V3NX();
	block->verts[faceOffset+0].colour = V4XYW();
	block->verts[faceOffset+1].colour = V4XYW();
	block->verts[faceOffset+2].colour = V4XYW();
	block->verts[faceOffset+3].colour = V4XYW();
	block->verts[faceOffset+0].position = Vec4(xPos - HalfBlockLength, yPos - HalfBlockLength, 0.f,    1.f);
	block->verts[faceOffset+1].position = Vec4(xPos - HalfBlockLength, yPos + HalfBlockLength, 0.f,    1.f);
	block->verts[faceOffset+2].position = Vec4(xPos - HalfBlockLength, yPos - HalfBlockLength, height, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos - HalfBlockLength, yPos + HalfBlockLength, height, 1.f);

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
	block->verts[faceOffset+1].position = Vec4(xPos + HalfBlockLength, yPos - HalfBlockLength, 0.f,    1.f);
	block->verts[faceOffset+0].position = Vec4(xPos + HalfBlockLength, yPos + HalfBlockLength, 0.f,    1.f);
	block->verts[faceOffset+3].position = Vec4(xPos + HalfBlockLength, yPos - HalfBlockLength, height, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos + HalfBlockLength, yPos + HalfBlockLength, height, 1.f);

	//-y
	faceOffset += 4;
	//block->verts[faceOffset+0].normal = V3NY();
	//block->verts[faceOffset+1].normal = V3NY();
	//block->verts[faceOffset+2].normal = V3NY();
	//block->verts[faceOffset+3].normal = V3NY();
	block->verts[faceOffset+0].colour = V4YZW();
	block->verts[faceOffset+1].colour = V4YZW();
	block->verts[faceOffset+2].colour = V4YZW();
	block->verts[faceOffset+3].colour = V4YZW();
	block->verts[faceOffset+0].position = Vec4(xPos - HalfBlockLength, yPos + HalfBlockLength, 0.f,    1.f);
	block->verts[faceOffset+1].position = Vec4(xPos + HalfBlockLength, yPos + HalfBlockLength, 0.f,    1.f);
	block->verts[faceOffset+2].position = Vec4(xPos - HalfBlockLength, yPos + HalfBlockLength, height, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos + HalfBlockLength, yPos + HalfBlockLength, height, 1.f);

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
	block->verts[faceOffset+1].position = Vec4(xPos - HalfBlockLength, yPos - HalfBlockLength, 0.f,    1.f);
	block->verts[faceOffset+0].position = Vec4(xPos + HalfBlockLength, yPos - HalfBlockLength, 0.f,    1.f);
	block->verts[faceOffset+3].position = Vec4(xPos - HalfBlockLength, yPos - HalfBlockLength, height, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos + HalfBlockLength, yPos - HalfBlockLength, height, 1.f);

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
	block->verts[faceOffset+1].position = Vec4(xPos - HalfBlockLength, yPos - HalfBlockLength, height, 1.f);
	block->verts[faceOffset+0].position = Vec4(xPos + HalfBlockLength, yPos - HalfBlockLength, height, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos - HalfBlockLength, yPos + HalfBlockLength, height, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos + HalfBlockLength, yPos + HalfBlockLength, height, 1.f);
}

Vec3 LifeGameCells::Block::BlockMin() const
{
	const Vec4& minVert = verts[0].position;
	return Vec3(minVert.x, minVert.y, minVert.z);
}

Vec3 LifeGameCells::Block::BlockMax() const
{
	const Vec4& maxVert = verts[6].position;
	return Vec3(maxVert.x, maxVert.y, maxVert.z);
}

void LifeGameCells::Initialise(beAppData* appData)
{
	auto renderInterface = appData->renderInterface;

	m_font.Init(renderInterface, appData->shaderPack, "tutefont.txt", "tutefont.dds");

	beRandom rng;
	rng.InitFromSystemTime();
	const int numBlocks = m_renderBlocks.Capacity();
	m_renderBlocks.SetCountUninitialised(numBlocks);

	for (int y : RangeIter(m_cells.Length()))
	{
		for (int x : RangeIter(m_cells.Length()))
		{
			const int arrayIndex = m_cells.ToIndex(x, y);

			bool live = rng.NextBool();
			m_cells[arrayIndex] = live;

			const float xPos = x*BlockLength;
			const float yPos = y*BlockLength;
			const float height = live ? BlockHeight : 0.f;
			//LOG("Writing to block[%d]", arrayIndex);
			InitBlock(&m_renderBlocks[arrayIndex], xPos, yPos, height);
		}
	}

	const int numVerts = numBlocks * Block::VertsPerBlock;
	//int numTris = numBlocks * Block::TrisPerBlock;
	bool success = m_vertexBuffer.Allocate(renderInterface, sizeof(GridVertexFormat), numVerts, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, m_renderBlocks.begin());
	BE_ASSERT(success);

	const u32 FaceIndexLayout[Block::IndicesPerFace] ={
		0, 1, 2, 2, 1, 3
	};

	beVector<u32> indices(numBlocks * Block::IndicesPerBlock);
	indices.SetCount(numBlocks * Block::IndicesPerBlock);
	for (int blockIndex : RangeIter(m_renderBlocks.Count()))
	{
		const int vertBlockStart = blockIndex * Block::VertsPerBlock;
		const int indexBlockStart = blockIndex * Block::IndicesPerBlock;

		for (int faceIndex : RangeIter(Block::FacesPerBlock))
		{
			const int vertFaceStart = vertBlockStart + (faceIndex * Block::VertsPerFace);
			const int indexFaceStart = indexBlockStart + (faceIndex * Block::IndicesPerFace);

			for (int indexIndex : RangeIter(Block::IndicesPerFace))
			{
				indices[indexFaceStart + indexIndex] = vertFaceStart + FaceIndexLayout[indexIndex];
			}
		}
	}

	success = m_indexBuffer.Allocate(renderInterface, ElementSize(indices), indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 0, indices.begin());
	BE_ASSERT(success);
}

void LifeGameCells::Finalise()
{
	m_indexBuffer.Release();
	m_vertexBuffer.Release();
	m_bitmapTextDynamic.Deinit();
	m_font.Deinit();
}

void LifeGameCells::Update(beAppData* appData, float dt, const Matrix& viewMatrix)
{
	// #TODO: I think this selection should happen in StateLifeGame
	auto mouse = appData->mouse;
	auto renderInterface = appData->renderInterface;
	if (auto posDir = beCameraUtils::GetScreeenToWorldRay(*renderInterface, viewMatrix, *mouse))
	{
		//LOG("Mouse pos:({:.3f}, {:.3f}, {:.3f}) Dir:({:.3f}, {:.3f}, {:.3f})\r\n", posDir->pos.x, posDir->pos.y, posDir->pos.z, posDir->dir.x, posDir->dir.y, posDir->dir.z);
		float minPosS = FLT_MAX;
		int collidingIndex = -1;
		// #TODO: Implement arrayiter
		for (auto iter : RangeIter(m_renderBlocks.Count()))
		{
			const Block& block = m_renderBlocks[iter];
			const Vec3& blockMin = block.BlockMin();
			const Vec3& blockMax = block.BlockMax();
			if (auto collisions = beIntersection::TRayVsAABB(posDir->pos, posDir->dir, blockMin, blockMax))
			{
				if (collisions->first >= 0.f)
				{
					minPosS = Min(minPosS, collisions->first);
					collidingIndex = iter;
				}
			}
		}
		if (collidingIndex != -1 && mouse->IsPressed(beMouse::Button::LeftButton))
		{
			m_cells.At(collidingIndex) = !m_cells.At(collidingIndex);
		}

		m_hightlightIndex = collidingIndex;
	}
	else
	{
		m_hightlightIndex = -1;
	}




	const float distance = m_animationDistancePerSecond * dt;
	const float negDist = -distance;

	for (auto iter : m_cells.GridIter())
	{
		Block* block = &m_renderBlocks[iter.index];
		const float increment = *iter ? distance : negDist;
		const float xPos = iter.xPos() * BlockLength;
		const float yPos = iter.yPos() * BlockLength;
		const bool highlight = iter.index == m_hightlightIndex;
		UpdateBlock(block, xPos, yPos, increment, highlight);
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
	for (int y : RangeIter(length))
	{
		const int yUp = (y-1+length) % length;
		const int yDown = (y+1+length) % length;
		for (int x : RangeIter(length))
		{
			const int xLeft = (x-1+length) % length;
			const int xRight = (x+1+length) % length;

			const int adjecentLivingCells =
				(int)m_cells.At(xLeft, yUp) +
				(int)m_cells.At(xLeft, y) +
				(int)m_cells.At(xLeft, yDown) +
				(int)m_cells.At(x, yUp) +
				(int)m_cells.At(x, yDown) +
				(int)m_cells.At(xRight, yUp) +
				(int)m_cells.At(xRight, y) +
				(int)m_cells.At(xRight, yDown)
			;
			//Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
			//Any live cell with two or three live neighbours lives on to the next generation.
			//Any live cell with more than three live neighbours dies, as if by overpopulation.
			//Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
			const int index = m_nextCells.ToIndex(x, y);
			if (adjecentLivingCells == 2)
			{
				m_nextCells[index] = m_cells[index];
			}
			else
			{
				m_nextCells[index] = adjecentLivingCells == 3;
			}
		}
	}
	
	m_cells = m_nextCells;
}

