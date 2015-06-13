#pragma once
#include "Core/beString.h"
#include "Math/beMath.h"
#include <d3d11.h>

class beRenderInterface;
struct ID3D11ShaderResourceView;

class beTexture : NonCopiable
{
public:

	beTexture();
	~beTexture();

	bool Init(beRenderInterface* ri, const beWString& textureFilename);
	bool InitAsTarget(beRenderInterface* ri, int height, int width);
	void FinaliseTarget();
	void Deinit();

	void Set(const beTexture& that); // inc refs.

	int GetWidth() const;
	int GetHeight() const;

	ID3D11ShaderResourceView* GetTexture();

	void SetAsTarget(beRenderInterface* ri);
	void Clear(beRenderInterface* ri, Vec4 clearColour);

private:
	D3D11_TEXTURE2D_DESC m_desc;
	ID3D11ShaderResourceView* m_texture;
	ID3D11Texture2D* m_texture2d;

	// When writable
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
};
