#include "bePCH.h"
#include "beTexture.h"

#include "Core\beAssert.h"
#include "Core\beString.h"
#include "Rendering\beRenderInterface.h"
#include "External/DirectXTK/DDSTextureLoader.h"

beTexture::beTexture()
	: m_texture(nullptr)
	, m_texture2d(nullptr)
	, m_renderTargetView(nullptr)
	, m_depthStencilView(nullptr)
	, m_depthStencilBuffer(nullptr)
{
}

beTexture::~beTexture()
{
	Deinit();
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

	ID3D11Resource* resource;
	m_texture->GetResource(&resource);
	resource->QueryInterface<ID3D11Texture2D>(&m_texture2d);
	m_texture2d->GetDesc(&m_desc);

	resource->Release();
	
	return true;
}

bool beTexture::InitAsTarget(beRenderInterface* ri, int height, int width)
{
	BE_ASSERT(!m_texture);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	auto device = ri->GetDevice();

	// Setup the render target texture description.
	ZeroMemory(&m_desc, sizeof(m_desc));
	m_desc.Width = width;
	m_desc.Height = height;
	m_desc.MipLevels = 1;
	m_desc.ArraySize = 1;
	m_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	m_desc.SampleDesc.Count = 1;
	m_desc.Usage = D3D11_USAGE_DEFAULT;
	m_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_desc.CPUAccessFlags = 0;
	m_desc.MiscFlags = 0;

	// Create the render target texture.
	HRESULT res = device->CreateTexture2D(&m_desc, NULL, &m_texture2d);
	if(FAILED(res)) { return false; }

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = m_desc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	res = device->CreateRenderTargetView(m_texture2d, &renderTargetViewDesc, &m_renderTargetView);
	if(FAILED(res)) { return false; }

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = m_desc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	res = device->CreateShaderResourceView(m_texture2d, &shaderResourceViewDesc, &m_texture);
	if(FAILED(res)) { return false; }
	
	D3D11_TEXTURE2D_DESC depthBufferDesc = {0};
	
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1; // Don't generate mips for our buffer because why would you want them
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	
	res = device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	res = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	return true;
}

void beTexture::FinaliseTarget()
{
	BE_SAFE_RELEASE(m_renderTargetView);
	BE_SAFE_RELEASE(m_depthStencilView);
	BE_SAFE_RELEASE(m_depthStencilBuffer);
}

void beTexture::Deinit()
{
	FinaliseTarget();
	BE_SAFE_RELEASE(m_texture2d);
	BE_SAFE_RELEASE(m_texture);
}

void beTexture::Set(const beTexture& that)
{

	Deinit();
	*this = that;
	m_texture->AddRef();
	m_texture2d->AddRef();
	if (m_renderTargetView)
	{
		m_renderTargetView->AddRef();
		m_depthStencilView->AddRef();
		m_depthStencilBuffer->AddRef();
	}
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

void beTexture::SetAsTarget(beRenderInterface * ri)
{
	BE_ASSERT(m_renderTargetView); // If this goes off, we weren't set up to be written to.
	ri->SetRenderTarget(m_renderTargetView, m_depthStencilView, (float)m_desc.Width, (float)m_desc.Height, 0.01f, 100.00f);
}

void beTexture::Clear(beRenderInterface* ri, Vec4 clearColour)
{
	auto context = ri->GetDeviceContext();
	context->ClearRenderTargetView(m_renderTargetView, &clearColour.x);

	// Maybe?
	//context->ClearDepthStencilView(ri->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
