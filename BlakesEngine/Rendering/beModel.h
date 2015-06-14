#pragma once
#include "BlakesEngine\Core\beString.h"
#include "BlakesEngine\Math\beMath.h"

class beRenderInterface;
class beTexture;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beModel
{
public:

	beModel();
	~beModel();

	bool Init(beRenderInterface* ri, const beWString& textureFilename);
	bool InitWithFilename(beRenderInterface* ri, const char* filename, const beWString& textureFilename);
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
