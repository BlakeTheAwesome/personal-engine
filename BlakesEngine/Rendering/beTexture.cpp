#include "bePCH.h"
#include "beTexture.h"

#include "Core\beAssert.h"
#include "Core\beString.h"
#include "Rendering\beRenderInterface.h"
#include "External/DirectXTK/DDSTextureLoader.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>

beTexture::beTexture()
	: m_texture(nullptr)
{
}

beTexture::~beTexture()
{
}

bool beTexture::Init(beRenderInterface* ri, const beWString& textureFilename)
{
	HRESULT res = DirectX::CreateDDSTextureFromFile(ri->GetDevice(), textureFilename.c_str(), nullptr, &m_texture);
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
