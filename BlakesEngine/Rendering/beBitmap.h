#pragma once
#include "beRenderBuffer.h"
#include "beTexture.h"
#include "beFont.h"
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Math/beMath.h"

class beRenderInterface;
class beTexture;
class beShaderPack;
struct ID3D11ShaderResourceView;

class beBitmap
{
public:
	beBitmap() = default;
	~beBitmap();

	bool Init(beRenderInterface* ri, const beTexture& texture);
	bool Init(beRenderInterface* ri, beShaderPack* shaderPack, float width, float height, const beWString& textureFilename);
	void Deinit();

	bool InitText(beRenderInterface* ri, const beFont* font, const beStringView& string, float scale, float maxWidth, u32 invalidStringCharacter, bool fixedWidth, beFont::WrapMode wrapMode);

	void SetColour(const Vec4& colour);
	Vec2 GetPosition() const;
	void SetPosition(float x, float y);
	void SetAnchorPoint(float x, float y); // [0-1]

	bool LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beWString& textureFilename);

	void Render(beRenderInterface* ri);
	
	ID3D11ShaderResourceView* GetTexture() const;

	int GetIndexCount();

private:
	bool InitCommon(beRenderInterface* ri, float width, float height);

	beTexture m_texture;
	beRenderBuffer m_vertexBuffer;
	beRenderBuffer m_indexBuffer;
	beRenderBuffer m_positionBuffer;

	Vec4 m_colour{1.f, 1.f, 1.f, 1.f};
	Vec2 m_size{0.f, 0.f};
	Vec2 m_position{0.f, 0.f};
	Vec2 m_anchorPoint{0.f, 0.f};
	bool m_dirtyPositionBuffer = true;
};
