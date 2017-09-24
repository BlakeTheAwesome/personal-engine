#pragma once
#include "BlakesEngine/framework/beState.h"
#include "BlakesEngine/Rendering/beBitmap.h"
#include "BlakesEngine/Rendering/beFont.h"
#include "BlakesEngine/Rendering/beRenderBuffer.h"
#include "BlakesEngine/Shaders/beShaderColour.h"
#include "BlakesEngine/Shaders/beShaderTexture2d.h"
#include "BlakesEngine/DataStructures/beFastGrid.h"
#include "BlakesEngine/DataStructures/beArray.h"
#include "BlakesEngine/Camera/beFlightCamera.h"

struct beAppData;
class beDebugWorld;
class beRenderInterface;
class beApData;
struct beShaderPack;

struct LifeGameCells
{
	static constexpr float BlockLength = 1.f;
	static constexpr float HalfBlockLength = BlockLength * 0.5f;
	static constexpr float BlockHeight = 1.f;

	~LifeGameCells() { Finalise(); }
	void Initialise(beAppData* appData);
	void Finalise();
	void TickGame();
	void Update(beAppData* appData, float hightOffset);
	void Render(beRenderInterface* renderInterface, beShaderPack* shaderPack, const Matrix& viewMatrix, const Vec3& cameraPosition);
	void SetAnimationSpeed(float distPerSecond) { m_animationDistancePerSecond = distPerSecond; }

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
	};

	static void UpdateBlock(LifeGameCells::Block* block, float xPos, float yPos, float height);
	static void InitBlock(LifeGameCells::Block* block, float xPos, float yPos, float height);

	beFont m_font;
	beBitmap m_bitmapTextDynamic;
	beFlightCamera m_camera;

	beFastGrid<bool, 32> m_cells, m_nextCells;
	beFixedVector<Block, 32*32> m_renderBlocks;
	beRenderBuffer m_vertexBuffer;
	beRenderBuffer m_indexBuffer;

	float m_animationDistancePerSecond = 1.f;
};
