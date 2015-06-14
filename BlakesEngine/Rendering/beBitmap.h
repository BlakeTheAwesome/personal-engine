#pragma once
#include "BlakesEngine\Core\beString.h"
#include "BlakesEngine\Math\beMath.h"

class beRenderInterface;
class beTexture;
class beFont;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beBitmap
{
public:

	beBitmap();
	~beBitmap();

	bool Init(beRenderInterface* ri, const beTexture& texture);
	bool Init(beRenderInterface* ri, float width, float height, const beWString& textureFilename);
	void Deinit();

	bool InitText(beRenderInterface* ri, const beFont* font, const beString& string, float maxWidth, u32 invalidStringCharacter);

	void SetColour(const Vec4& colour);
	Vec2 GetPosition() const;
	void SetPosition(float x, float y);
	void SetAnchorPoint(float x, float y); // [0-1]

	bool LoadTexture(beRenderInterface* ri, const beWString& textureFilename);

	void Render(beRenderInterface* ri);
	
	ID3D11ShaderResourceView* GetTexture() const;

	int GetIndexCount();

private:
	bool InitCommon(beRenderInterface* ri, float width, float height);

	beTexture* m_texture;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_positionBuffer;
	int m_vertexCount;
	int m_indexCount;

	Vec4 m_colour;
	Vec2 m_size;
	Vec2 m_position;
	Vec2 m_anchorPoint;
	bool m_dirtyPositionBuffer;
};
