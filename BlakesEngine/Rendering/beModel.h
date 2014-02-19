#pragma once
#include "Core\beString.h"
#include "Math\beMath.h"

class beRenderInterface;
class beTexture;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beModel
{
public:

	beModel();
	~beModel();

	bool Init(beRenderInterface* ri);
	bool InitWithFilename(beRenderInterface* ri, const char* filename);
	void Deinit();

	bool LoadTexture(beRenderInterface* ri, const beWString& textureFilename);

	void Render(beRenderInterface* ri);
	
	ID3D11ShaderResourceView* GetTexture() const;

	int GetIndexCount();

private:
	beTexture* m_texture;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
};
