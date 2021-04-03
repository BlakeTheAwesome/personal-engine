module;
#include "BlakesEngine/Core/beString.h"
#include <d3d11.h>

export module beBitmap;

import beTexture;
import beFont;
import beRenderBuffer;
import beMath;
import beShaderPack;
import beRenderInterface;
import beTypes;


export class beBitmap
{
public:
	beBitmap() = default;
	~beBitmap();

	beBitmap(const beBitmap&) = delete;
	beBitmap(beBitmap&&) = delete;
	beBitmap& operator=(const beBitmap&) = delete;
	beBitmap& operator=(beBitmap&&) = delete;


	bool Init(beRenderInterface* ri, const beTexture& texture);
	bool Init(beRenderInterface* ri, beShaderPack* shaderPack, float width, float height, const beStringView& textureFilename);
	void Deinit();

	bool InitText(beRenderInterface* ri, const beFont* font, const beStringView& string, float scale, float maxWidth, u32 invalidStringCharacter, bool fixedWidth, beFont::WrapMode wrapMode);

	void SetColour(const Vec4& colour);
	Vec2 GetPosition() const;
	void SetPosition(float x, float y);
	void SetPosition(const Vec2& v);
	void SetAnchorPoint(float x, float y); // [0-1]

	bool LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename);

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
