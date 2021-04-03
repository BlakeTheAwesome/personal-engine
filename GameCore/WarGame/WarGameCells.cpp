module;
#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Math/beRandomFunctions.h"
#include <d3d11.h>

module WarGameCells;

import beShaderPack;
import beMath;
import beRandom;
import beCameraUtils;
import beGamepad;
import beKeyboard;
import beMouse;
import beIntersection;
import beRenderInterface;
import beContainerHelpers;
import beAppData;
import beZOrder;

static constexpr int MAX_HP = 5;

void WarGameCells::Render(beRenderInterface* renderInterface, beShaderPack* shaderPack, const Matrix& viewMatrix, const Vec3& cameraPosition)
{
	shaderPack->shaderColour.SetShaderParameters(renderInterface);

	ID3D11DeviceContext* deviceContext = renderInterface->GetDeviceContext();
	unsigned int stride = m_vertexBuffer.ElementSize();
	unsigned int offset = 0;

	ID3D11Buffer* vertexBuffers[] ={m_vertexBuffer.GetBuffer()};
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_indexBuffer.D3DIndexTopology());
	shaderPack->shaderColour.SetActive(renderInterface);
	shaderPack->shaderColour.Render(renderInterface, m_indexBuffer.NumElements(), 0);
/*
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
				char c = m_cells.At(x, y) ? 'o' : 'x';
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
	}*/
}

static Vec4 GetColour(WarGameCells::WarCell const& cell, float strength)
{
	Vec4 colour = V4W();
	switch (cell.colour)
	{
		case WarGameCells::TeamColour::Blue:
		{
			colour = V4ZW();
		}
		break;

		case WarGameCells::TeamColour::Red:
		{
			colour = V4XZ();
		}
		break;

		default: break;
	}

	return colour * Vec4{strength, strength, strength, 1.f};
}

void WarGameCells::UpdateBlock(WarGameCells::Block* block, WarCell const& cell, float xPos, float yPos, float distance, bool highlight)
{
	float currentHeight = block->verts[2].position.z;
	float newHeight = Clamp(currentHeight+distance, 0.f, BlockHeight * cell.hp);

	Vec4 colour = GetColour(cell, newHeight);

	//-x
	int faceOffset = 0;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;

	//+x
	faceOffset += 4;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;

	//-y
	faceOffset += 4;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;

	//+y
	faceOffset += 4;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;

	//+z
	faceOffset += 4;
	block->verts[faceOffset+0].position.z = newHeight;
	block->verts[faceOffset+1].position.z = newHeight;
	block->verts[faceOffset+2].position.z = newHeight;
	block->verts[faceOffset+3].position.z = newHeight;
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;

	if (highlight)
	{
		for (GridVertexFormat& vert : block->verts)
		{
			vert.colour = V41();
		}
	}
}

void WarGameCells::InitBlock(WarGameCells::Block* block, WarCell const& cell, float xPos, float yPos, float height)
{
	Vec4 colour = GetColour(cell, height);

	//-x
	int faceOffset = 0;
	//block->verts[faceOffset+0].normal = V3NX();
	//block->verts[faceOffset+1].normal = V3NX();
	//block->verts[faceOffset+2].normal = V3NX();
	//block->verts[faceOffset+3].normal = V3NX();
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;
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
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;
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
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;
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
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;
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
	block->verts[faceOffset+0].colour = colour;
	block->verts[faceOffset+1].colour = colour;
	block->verts[faceOffset+2].colour = colour;
	block->verts[faceOffset+3].colour = colour;
	block->verts[faceOffset+1].position = Vec4(xPos - HalfBlockLength, yPos - HalfBlockLength, height, 1.f);
	block->verts[faceOffset+0].position = Vec4(xPos + HalfBlockLength, yPos - HalfBlockLength, height, 1.f);
	block->verts[faceOffset+3].position = Vec4(xPos - HalfBlockLength, yPos + HalfBlockLength, height, 1.f);
	block->verts[faceOffset+2].position = Vec4(xPos + HalfBlockLength, yPos + HalfBlockLength, height, 1.f);
}

Vec3 WarGameCells::Block::BlockMin() const
{
	const Vec4& minVert = verts[0].position;
	return Vec3(minVert.x, minVert.y, minVert.z);
}

Vec3 WarGameCells::Block::BlockMax() const
{
	const Vec4& maxVert = verts[6].position;
	return Vec3(maxVert.x, maxVert.y, maxVert.z);
}

void WarGameCells::Initialise(beAppData* appData)
{
	auto renderInterface = appData->renderInterface;

	m_font.Init(renderInterface, appData->shaderPack, "tutefont.txt", "tutefont.dds");
	int numBlocks = m_renderBlocks.Capacity();
	m_renderBlocks.SetCountUninitialised(numBlocks);

	Restart();

	int numVerts = numBlocks * Block::VertsPerBlock;
	//int numTris = numBlocks * Block::TrisPerBlock;
	bool success = m_vertexBuffer.Allocate(renderInterface, sizeof(GridVertexFormat), numVerts, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, m_renderBlocks.begin());
	BE_ASSERT(success);

	u32 FaceIndexLayout[Block::IndicesPerFace] ={
		0, 1, 2, 2, 1, 3
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

	success = m_indexBuffer.Allocate(renderInterface, ElementSize(indices), indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 0, indices.begin());
	BE_ASSERT(success);
}

void WarGameCells::Restart()
{
	beRandom rng;
	rng.InitFromSystemTime();

	beArray<int, TeamColour::Count> teamCounts{0};
	for (int i : RangeIter(teamCounts.size()))
	{
		LOG("Colour {} count: {}", i, teamCounts[i]);
	}
	for (int y : RangeIter(m_cells.Length()))
	{
		for (int x : RangeIter(m_cells.Length()))
		{
			int arrayIndex = m_cells.ToIndex(x, y);
			WarCell& cell = m_cells[arrayIndex];
			bool live = rng.NextBool();
			if (live)
			{
				int colourIndex = rng.Next(1, (int)TeamColour::Count-1);
				if (colourIndex == 1)
				{
					colourIndex = rng.Next(1, (int)TeamColour::Count-1);
				}
				if (colourIndex == 1)
				{
					colourIndex = rng.Next(1, (int)TeamColour::Count-1);
				}

				teamCounts[colourIndex]++;
				cell.colour = (TeamColour)colourIndex;
				cell.hp = 1;
			}
			else
			{
				cell.colour = TeamColour::None;
				cell.hp = 0;
			}

			float xPos = x*BlockLength;
			float yPos = y*BlockLength;
			float height = live ? BlockHeight : 0.f;
			//LOG("Writing to block[%d]", arrayIndex);
			InitBlock(&m_renderBlocks[arrayIndex], cell, xPos, yPos, height);
		}
	}
	for (int i : RangeIter(teamCounts.size()))
	{
		LOG("Colour {} count: {}", i, teamCounts[i]);
	}
}

void WarGameCells::Finalise()
{
	m_indexBuffer.Release();
	m_vertexBuffer.Release();
	m_bitmapTextDynamic.Deinit();
	m_font.Deinit();
}

void WarGameCells::Update(beAppData* appData, float dt, const Matrix& viewMatrix)
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
			m_cells.At(collidingIndex).hp ^= 1;
		}

		m_hightlightIndex = collidingIndex;
	}
	else
	{
		m_hightlightIndex = -1;
	}




	float distance = m_animationDistancePerSecond * dt;
	float negDist = -distance;

	// ICE's in VC 16.9.3
	//for (auto iter : m_cells.GridIter())
	//{
	//	Block* block = &m_renderBlocks[iter.index];
	//	float increment = (*iter).hp != 0 ? distance : negDist;
	//	float xPos = iter.xPos() * BlockLength;
	//	float yPos = iter.yPos() * BlockLength;
	//	bool highlight = iter.index == m_hightlightIndex;
	//	UpdateBlock(block, *iter, xPos, yPos, increment, highlight);
	//}
	for (int index : RangeIter(m_cells.Capacity()))
	{
		Block* block = &m_renderBlocks[index];
		const float increment = m_cells.At(index).hp ? distance : negDist;
		const float xPos = beZOrder::DecodeMorton2X(index) * BlockLength;
		const float yPos = beZOrder::DecodeMorton2Y(index) * BlockLength;
		const bool highlight = index == m_hightlightIndex;
		UpdateBlock(block, m_cells.At(index), xPos, yPos, increment, highlight);
	}


	{
		void* bufferMem = m_vertexBuffer.Map(renderInterface);
		BE_ASSERT(m_vertexBuffer.BufferSize() == m_renderBlocks.DataSize());
		BE_MEMCPY(bufferMem, m_renderBlocks.begin(), m_renderBlocks.DataSize());
		m_vertexBuffer.Unmap(renderInterface);
	}
}

void WarGameCells::TickGame()
{
	beArray<int, TeamColour::Count> teamCountTest{0};
	for (WarCell const& cell : m_cells)
	{
		teamCountTest[(int)cell.colour]++;
	}
	LOG("PRE - Dead:{} Blue:{} Red:{} Count:{} Size:{}", teamCountTest[0], teamCountTest[1], teamCountTest[2], std::accumulate(teamCountTest.begin(), teamCountTest.end(), 0), m_cells.Capacity());

	teamCountTest.SetAllTo(0);
	beArray<int, TeamColour::Count> copiedColours{0};
	beArray<int, TeamColour::Count> takeoverColours{0};
	beArray<int, TeamColour::Count> dyingColours{0};
	beRandom rng;
	const int length = m_cells.Length();
	for (int y : RangeIter(length))
	{
		int yUp = (y-1+length) % length;
		int yDown = (y+1+length) % length;
		for (int x : RangeIter(length))
		{
			int index = m_nextCells.ToIndex(x, y);
			WarCell const& currentCell = m_cells[index];

			int xLeft = (x-1+length) % length;
			int xRight = (x+1+length) % length;

			beArray<int, TeamColour::Count> teamCounts{0};
			//teamCounts[(int)TeamColour::None] = 3;
			/*for (int i : RangeIter(1, (int)TeamColour::Count))
			{
				teamCounts[i] = 1;
			}*/
			teamCounts[(int)currentCell.colour] += 4;

			auto AddCell = [&](WarCell const& cell)
			{
				teamCounts[(int)cell.colour]++;
			};
			AddCell(m_cells.At(xLeft, yUp));
			AddCell(m_cells.At(xLeft, y));
			AddCell(m_cells.At(xLeft, yDown));
			AddCell(m_cells.At(x, yUp));
			AddCell(m_cells.At(x, yDown));
			AddCell(m_cells.At(xRight, yUp));
			AddCell(m_cells.At(xRight, y));
			AddCell(m_cells.At(xRight, yDown));


			WarCell* newCell = &m_nextCells[index];
			*newCell = currentCell;
			copiedColours[newCell->colour]++;

			auto newColour = (TeamColour)beRandomFunctions::WeightedRandom(&rng, teamCounts);
			teamCountTest[newColour]++;
			if (newColour == TeamColour::None)
			{
				newCell->hp /= 2;
				if (newCell->hp == 0)
				{
					//LOG("DyingCell:{} now {}", (int)newCell->colour, (int)newColour);
					dyingColours[newCell->colour]++;
					newCell->colour = newColour;
				}
			}
			else if (newColour == currentCell.colour)
			{
				newCell->hp = Min(currentCell.hp+1, MAX_HP);
			}
			else
			{
				newCell->hp = currentCell.hp - 1;
				if (newCell->hp <= 0)
				{
					//LOG("Takeover Cell:{} now {}", (int)newCell->colour, (int)newColour);
					newCell->hp = 1;
					newCell->colour = newColour;
					takeoverColours[newColour]++;
				}
			}
		}
	}
	LOG("CopiedColours - Dead:{} Blue:{} Red:{} Count:{}", copiedColours[0], copiedColours[1], copiedColours[2], std::accumulate(copiedColours.begin(), copiedColours.end(), 0));
	LOG("DyingColours - Dead:{} Blue:{} Red:{} Count:{}", dyingColours[0], dyingColours[1], dyingColours[2], std::accumulate(dyingColours.begin(), dyingColours.end(), 0));
	LOG("TakeoverColours - Dead:{} Blue:{} Red:{} Count:{}", takeoverColours[0], takeoverColours[1], takeoverColours[2], std::accumulate(takeoverColours.begin(), takeoverColours.end(), 0));
	LOG("NewColours - Dead:{} Blue:{} Red:{} Count:{}", teamCountTest[0], teamCountTest[1], teamCountTest[2], std::accumulate(teamCountTest.begin(), teamCountTest.end(), 0));

	teamCountTest.SetAllTo(0);
	for (WarCell const& cell : m_nextCells)
	{
		teamCountTest[(int)cell.colour]++;
	}
	LOG("Next - Dead:{} Blue:{} Red:{} Count:{} Size:{}", teamCountTest[0], teamCountTest[1], teamCountTest[2], std::accumulate(teamCountTest.begin(), teamCountTest.end(), 0), m_nextCells.Capacity());
	m_cells = m_nextCells;

	teamCountTest.SetAllTo(0);
	for (WarCell const& cell : m_cells)
	{
		teamCountTest[(int)cell.colour]++;
	}
	LOG("POST - Dead:{} Blue:{} Red:{} Count:{} Size:{}", teamCountTest[0], teamCountTest[1], teamCountTest[2], std::accumulate(teamCountTest.begin(), teamCountTest.end(), 0), m_cells.Capacity());

}

