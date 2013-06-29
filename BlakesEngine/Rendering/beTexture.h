#pragma once
#include "Core/beString.h"

class beRenderInterface;
struct ID3D11ShaderResourceView;

class beTexture
{
public:

	beTexture();
	~beTexture();

	bool Init(beRenderInterface* ri, const beWString& textureFilename);
	void Deinit();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};
