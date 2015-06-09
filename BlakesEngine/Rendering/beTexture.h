#pragma once
#include "Core/beString.h"
#include <d3d11.h>

class beRenderInterface;
struct ID3D11ShaderResourceView;

class beTexture : NonCopiable
{
public:

	beTexture();
	~beTexture();

	bool Init(beRenderInterface* ri, const beWString& textureFilename);
	void Deinit();

	void Set(const beTexture& that); // inc refs.

	int GetWidth() const;
	int GetHeight() const;

	ID3D11ShaderResourceView* GetTexture();

private:
	D3D11_TEXTURE2D_DESC m_desc;
	ID3D11ShaderResourceView* m_texture;
	ID3D11Texture2D* m_texture2d;
};
