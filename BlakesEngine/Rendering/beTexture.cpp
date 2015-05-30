#include "bePCH.h"
#include "beTexture.h"

#include "Core\beAssert.h"
#include "Core\beString.h"
#include "Rendering\beRenderInterface.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx11tex.h>
#include <d3dx10.h>

beTexture::beTexture()
	: m_texture(nullptr)
{
}

beTexture::~beTexture()
{
}

bool beTexture::Init(beRenderInterface* ri, const beWString& textureFilename)
{
	HRESULT res = D3DX11CreateShaderResourceViewFromFile(ri->GetDevice(), textureFilename.c_str(), nullptr, nullptr, &m_texture, nullptr);
	if(FAILED(res))
	{
		BE_ASSERT(false);
		return false;
	}

	return true;
}

void beTexture::Deinit()
{
	BE_SAFE_RELEASE(m_texture);
}

ID3D11ShaderResourceView* beTexture::GetTexture()
{
	return m_texture;
}
