#include "bePCH.h"
#include "beTexture.h"

#include "Core\beAssert.h"
#include "Core\beString.h"
#include "Rendering\beRenderInterface.h"
#include "External/DirectXTK/DDSTextureLoader.h"

beTexture::beTexture()
	: m_texture(nullptr)
	, m_texture2d(nullptr)
{
}

beTexture::~beTexture()
{
	BE_SAFE_RELEASE(m_texture2d);
	BE_SAFE_RELEASE(m_texture);
}

bool beTexture::Init(beRenderInterface* ri, const beWString& textureFilename)
{
	BE_ASSERT(!m_texture);

	HRESULT res = DirectX::CreateDDSTextureFromFile(ri->GetDevice(), textureFilename.c_str(), nullptr, &m_texture);
	if(FAILED(res))
	{
		BE_ASSERT(false);
		return false;
	}

	m_texture->QueryInterface<ID3D11Texture2D>(&m_texture2d);
	m_texture2d->GetDesc(&m_desc);


	return true;
}

void beTexture::Deinit()
{
	BE_SAFE_RELEASE(m_texture);
}

int beTexture::GetWidth() const
{
	return (int)m_desc.Width;
}

int beTexture::GetHeight() const
{
	return (int)m_desc.Height;
}

ID3D11ShaderResourceView* beTexture::GetTexture()
{
	return m_texture;
}
