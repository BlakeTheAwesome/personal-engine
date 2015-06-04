#pragma once
#include "Core\beString.h"
#include "Math\beMath.h"

class beRenderInterface;
class beTexture;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beBitmap
{
public:

	beBitmap();
	~beBitmap();

	bool Init(beRenderInterface* ri, float width, float height);
	void Deinit();

	Vec2 GetPosition() const;
	void SetPosition(float x, float y);
	void SetAnchorPoint(float x, float y); // [0-1]

	bool LoadTexture(beRenderInterface* ri, const beWString& textureFilename);

	void Render(beRenderInterface* ri);
	
	ID3D11ShaderResourceView* GetTexture() const;

	int GetIndexCount();

private:
	beTexture* m_texture;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_positionBuffer;
	int m_vertexCount;
	int m_indexCount;

	Vec2 m_size;
	Vec2 m_position;
	Vec2 m_anchorPoint;
	bool m_dirtyPositionBuffer;
};
