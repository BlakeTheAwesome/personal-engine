#pragma once
#include "BlakesEngine/Rendering/beBitmap.h"
#include "BlakesEngine/Rendering/beFont.h"
#include "BlakesEngine/Rendering/beRenderBuffer.h"
#include "BlakesEngine/Shaders/beShaderColour.h"
#include "BlakesEngine/Shaders/beShaderTexture2d.h"
#include "BlakesEngine/DataStructures/beFastGrid.h"

import beArray;
import beStateMachine;
import beFlightCamera;

struct beAppData;
class beDebugWorld;
class beRenderInterface;
class beApData;
class beShaderPack;

struct LifeGameCells
{
	static constexpr float BlockLength = 1.f;
	static constexpr float HalfBlockLength = BlockLength * 0.5f;
	static constexpr float BlockHeight = 1.f;

	LifeGameCells() = default;
	~LifeGameCells() { Finalise(); }

	LifeGameCells(const LifeGameCells&) = delete;
	LifeGameCells(LifeGameCells&&) = delete;
	LifeGameCells& operator=(const LifeGameCells&) = delete;
	LifeGameCells& operator=(LifeGameCells&&) = delete;

	void Initialise(beAppData* appData);
	void Finalise();
	void TickGame();
	void Update(beAppData* appData, float hightOffset, const Matrix& viewMatrix);
	void Render(beRenderInterface* renderInterface, beShaderPack* shaderPack, const Matrix& viewMatrix, const Vec3& cameraPosition);
	void SetAnimationSpeed(float distPerSecond) { m_animationDistancePerSecond = distPerSecond; }
	void ToggleRenderText() { m_renderTextCells = !m_renderTextCells; }
	void KillAll() { m_cells.SetAll(false); }

	private:

	using GridVertexFormat = beShaderColour::VertexType;
	//struct GridVertexFormat
	//{
	//	//u32 dataIndex;
	//	Vec3 normal;
	//	Vec2 pos;
	//	float height;
	//};
	struct Block
	{
		static constexpr int FacesPerBlock = 5;
		static constexpr int TrisPerFace = 2;
		static constexpr int VertsPerFace = 4;
		static constexpr int IndicesPerFace = TrisPerFace * 3;
		static constexpr int VertsPerBlock = FacesPerBlock * VertsPerFace;
		static constexpr int TrisPerBlock = FacesPerBlock * TrisPerFace;
		static constexpr int IndicesPerBlock = TrisPerBlock * 3;

		beArray<GridVertexFormat, VertsPerBlock> verts;
		Vec3 BlockMin() const;
		Vec3 BlockMax() const;
	};

	static void UpdateBlock(LifeGameCells::Block* block, float xPos, float yPos, float height, bool highlight);
	static void InitBlock(LifeGameCells::Block* block, float xPos, float yPos, float height);

	beFont m_font;
	beBitmap m_bitmapTextDynamic;

	static constexpr int LENGTH = 64;
	beFastGrid<bool, LENGTH> m_cells, m_nextCells;
	beFixedVector<Block, LENGTH*LENGTH> m_renderBlocks;
	beRenderBuffer m_vertexBuffer;
	beRenderBuffer m_indexBuffer;

	int m_hightlightIndex = -1;
	float m_animationDistancePerSecond = 2.f;
	bool m_renderTextCells = false;
};
