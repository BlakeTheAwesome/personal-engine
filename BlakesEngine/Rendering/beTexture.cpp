#include "BlakesEngine/bePCH.h"
#include "beTexture.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/beStringUtil.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/External/DirectXTK/DDSTextureLoader.h"
#include "BlakesEngine/External/stb/stb_image.h"
#include "BlakesEngine/Shaders/beShaderPack.h"

#include <D3D11.h>
#include <filesystem>

beTexture::~beTexture()
{
	Deinit();
}

bool beTexture::Init(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename, optional_arg<LoadOptions> loadOptions)
{
	BE_ASSERT(!m_texture);

	enum FileType
	{
		Invalid = 0,
		DDS,
		PNG,
	};

	FileType fileType = Invalid;
	{
		auto fileNameBegin = textureFilename.begin();
		auto fileNameEnd = textureFilename.end();
		int extensionIndex = beStringUtil::FindLast(fileNameBegin, fileNameEnd, '.');
		if (extensionIndex == -1)
		{
			BE_ASSERT(false);
			return false;
		}
		auto fileNameExt = fileNameBegin + extensionIndex + 1;

		if (beStringUtil::IsEqualI(fileNameExt, fileNameEnd, "dds"))
		{
			fileType = DDS;
		}
		else if (beStringUtil::IsEqualI(fileNameExt, fileNameEnd, "PNG"))
		{
			fileType = PNG;
		}
	}

	const char* additionalLoadDir = nullptr;
	if (loadOptions)
	{
		const LoadOptions& options = *loadOptions;
		if (!options.additionalLoadDir.empty())
		{
			additionalLoadDir = options.additionalLoadDir.c_str();
		}
	}
	if (fileType == DDS)
	{
		beWString filename = beStringConversion::UTF8ToWide(textureFilename);
		HRESULT res = DirectX::CreateDDSTextureFromFile(ri->GetDevice(), filename.c_str(), nullptr, &m_texture);
		if (FAILED(res))
		{
			if (additionalLoadDir)
			{
				std::filesystem::path path(additionalLoadDir);
				path /= textureFilename.c_str();
				res = DirectX::CreateDDSTextureFromFile(ri->GetDevice(), path.c_str(), nullptr, &m_texture);
			}
			if (FAILED(res))
			{
				BE_ASSERT(false);
				return false;
			}
		}
		ID3D11Resource* resource;
		m_texture->GetResource(&resource);
		resource->QueryInterface<ID3D11Texture2D>(&m_texture2d);
		m_texture2d->GetDesc(&m_desc);
		resource->Release();
	}
	else
	{
		int imageWidth, imageHeight, channelsInFile;
		auto imageBuffer = stbi_load(textureFilename.c_str(), &imageWidth, &imageHeight, &channelsInFile, 4);
		if (!imageBuffer && additionalLoadDir)
		{
			std::filesystem::path path(additionalLoadDir);
			path /= textureFilename.c_str();
			const beString& u8Path = path.u8string();
			imageBuffer = stbi_load(u8Path.c_str(), &imageWidth, &imageHeight, &channelsInFile, 4);
		}
		BE_ASSERT(imageBuffer);
		defer(stbi_image_free(imageBuffer););

		DXGI_SAMPLE_DESC sampleDesc;
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = imageWidth;
		desc.Height = imageHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.SampleDesc = sampleDesc;

		D3D11_SUBRESOURCE_DATA loadInfo;
		loadInfo.pSysMem = imageBuffer;
		loadInfo.SysMemPitch = imageWidth * 4;
		loadInfo.SysMemSlicePitch = imageWidth * imageHeight * 4;

		m_desc = desc;

		auto device = ri->GetDevice();
		HRESULT res = device->CreateTexture2D(&m_desc, &loadInfo, &m_texture2d);
		BE_ASSERT(SUCCEEDED(res));
		res = device->CreateShaderResourceView(m_texture2d, nullptr, &m_texture);
		BE_ASSERT(SUCCEEDED(res));
	}

	if (loadOptions)
	{
		const LoadOptions& options = *loadOptions;
		int currentHeight = m_desc.Height;
		int currentWidth = m_desc.Width;
		int currentMipLevel = m_desc.MipLevels;

		bool textureMatches = true;
		textureMatches = textureMatches && (options.height == 0 || options.height == currentHeight);
		textureMatches = textureMatches && (options.width == 0  || options.width  == currentWidth);
		textureMatches = textureMatches && (options.mipLevels == 0  || options.mipLevels == currentMipLevel);
		textureMatches = textureMatches && (!options.cpuReadable || (m_desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ) != 0);
		textureMatches = textureMatches && (!options.cpuWritable || (m_desc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE) != 0);
		if (textureMatches)
		{
			switch (m_desc.Format)
			{
				case DXGI_FORMAT_R8G8B8A8_UNORM: textureMatches = options.format == beTextureFormat::R8G8B8A8_UNORM; break;
				case DXGI_FORMAT_R32G32B32_FLOAT: textureMatches = options.format == beTextureFormat::R32G32B32_FLOAT; break;
				default: textureMatches = false;
			}
		}

		if (!textureMatches)
		{
			//Convert into desired format
			auto loadedTexture = m_texture;
			auto loadedTexture2d = m_texture2d;
			m_texture = nullptr;
			m_texture2d = nullptr;
			
			LoadOptions targetOptions = options;
			if (targetOptions.height == 0)
			{
				targetOptions.height = m_desc.Height;
			}
			if (targetOptions.width == 0)
			{
				targetOptions.width = m_desc.Width;
			}
			InitAsTarget(ri, targetOptions);
			SetAsTarget(ri);
			// Render into new texture
			
			shaderPack->shaderTexture2d.RenderQuad(ri, V2N1(), V21(), false, loadedTexture, true, beShaderTexture2d::TextureMode::Clamped);
			ri->RestoreRenderTarget();
			FinaliseTarget();
			loadedTexture->Release();
			loadedTexture2d->Release();
		}
	}
	
	return true;
}

bool beTexture::InitAsTarget(beRenderInterface* ri, const LoadOptions& loadOptions)
{
	BE_ASSERT(!m_texture);

	BE_ASSERT(loadOptions.height > 0);
	BE_ASSERT(loadOptions.width > 0);
	BE_ASSERT(loadOptions.format != beTextureFormat::Invalid);

	int height = loadOptions.height;
	int width = loadOptions.width;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	switch (loadOptions.format)
	{
		case beTextureFormat::R32G32B32_FLOAT: format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		case beTextureFormat::R8G8B8A8_UNORM: format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		default: break;
	}
	BE_ASSERT(format != DXGI_FORMAT_UNKNOWN);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	auto device = ri->GetDevice();

	// Setup the render target texture description.
	ZeroMem(&m_desc);
	m_desc.Width = width;
	m_desc.Height = height;
	m_desc.MipLevels = loadOptions.mipLevels > 0 ? loadOptions.mipLevels : 1;
	m_desc.ArraySize = 1;
	m_desc.Format = format;
	m_desc.SampleDesc.Count = 1;
	m_desc.Usage = D3D11_USAGE_DEFAULT;
	m_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_desc.CPUAccessFlags = 0;
	m_desc.MiscFlags = 0;

	// Create the render target texture.
	HRESULT res = device->CreateTexture2D(&m_desc, nullptr, &m_texture2d);
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
	shaderResourceViewDesc.Texture2D.MipLevels = m_desc.MipLevels;

	// Create the shader resource view.
	res = device->CreateShaderResourceView(m_texture2d, &shaderResourceViewDesc, &m_texture);
	if(FAILED(res)) { return false; }
	
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	
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
	ZeroMem(&depthStencilViewDesc);
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

ID3D11ShaderResourceView* beTexture::GetTexture() const
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
	context->ClearDepthStencilView(ri->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

beTexture::beTexture(beTexture&& rhs) noexcept
{
	*this = rhs;
	rhs.m_texture = nullptr;
	rhs.m_texture2d = nullptr;
	rhs.m_renderTargetView = nullptr;
	rhs.m_depthStencilBuffer = nullptr;
	rhs.m_depthStencilView = nullptr;
}

beTexture& beTexture::operator=(beTexture&& rhs) noexcept
{
	Deinit();
	*this = rhs;
	rhs.m_texture = nullptr;
	rhs.m_texture2d = nullptr;
	rhs.m_renderTargetView = nullptr;
	rhs.m_depthStencilBuffer = nullptr;
	rhs.m_depthStencilView = nullptr;
	return *this;
}
