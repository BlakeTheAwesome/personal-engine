#pragma once
#include "BlakesEngine/Framework/beState.h"
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
class beShaderPack;

class WarGameCells
{
	public:
	enum class Team
	{
		Red,
		Blue,

		Count
	};

	static constexpr float BlockLength = 1.f;
	static constexpr float HalfBlockLength = BlockLength * 0.5f;
	static constexpr float BlockHeight = 0.3f;

	~WarGameCells() { Finalise(); }
	void Initialise(beAppData* appData);
	void Restart();
	void Finalise();
	void TickGame();
	void Update(beAppData* appData, float hightOffset, const Matrix& viewMatrix);
	void Render(beRenderInterface* renderInterface, beShaderPack* shaderPack, const Matrix& viewMatrix, const Vec3& cameraPosition);
	void SetAnimationSpeed(float distPerSecond) { m_animationDistancePerSecond = distPerSecond; }
	void ToggleRenderText() { m_renderTextCells = !m_renderTextCells; }
	void KillAll() { m_cells.SetAll({}); }


	enum class TeamColour : u8
	{
		None,
		Blue,
		Red,

		Count
	};

	struct WarCell
	{
		int hp = 0;
		TeamColour colour = TeamColour::None;
	};

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

	static void UpdateBlock(WarGameCells::Block* block, WarCell const& cell, float xPos, float yPos, float height, bool highlight);
	static void InitBlock(WarGameCells::Block* block, WarCell const& cell, float xPos, float yPos, float height);

	beFont m_font;
	beBitmap m_bitmapTextDynamic;


	static constexpr int LENGTH = 64;
	beFastGrid<WarCell, LENGTH> m_cells, m_nextCells;
	beFixedVector<Block, LENGTH*LENGTH> m_renderBlocks;
	beRenderBuffer m_vertexBuffer;
	beRenderBuffer m_indexBuffer;

	int m_hightlightIndex = -1;
	float m_animationDistancePerSecond = 2.f;
	bool m_renderTextCells = false;
};
