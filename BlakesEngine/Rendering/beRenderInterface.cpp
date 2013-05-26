#include "bePCH.h"
#include "beRenderInterface.h"

#include "Core/beMacros.h"
#include "Core/beAssert.h"

#include "Window/beWindow.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

class beRenderInterface::Impl
{
	public:
	Impl();
	~Impl();

	void CreateDevice(HWND* hWnd);
	void CreateBackBuffer();
	void InitialiseViewport(int width, int height);

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device; // Mostly memory like stuff
	ID3D11DeviceContext* m_deviceContext; // Mostly GPUish stuff
	ID3D11RenderTargetView *m_backBuffer;
};

BE_PIMPL_CPP_DECLARE(beRenderInterface)
{
	BE_PIMPL_CPP_BODY(beRenderInterface);
}

beRenderInterface::Impl::Impl()
	: m_swapChain(NULL)
	, m_device(NULL)
	, m_deviceContext(NULL)
	, m_backBuffer(NULL)
{
}

beRenderInterface::Impl::~Impl()
{
}

void beRenderInterface::Init(beWindow* window)
{
	HWND* hWnd = (HWND*)window->GetHWnd();
	m_impl->CreateDevice(hWnd);
	m_impl->CreateBackBuffer();
	m_impl->InitialiseViewport(window->GetWidth(), window->GetHeight());
}

void beRenderInterface::Impl::CreateDevice(HWND* hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd = {0};
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = *hWnd;
	scd.Windowed = TRUE;
	scd.SampleDesc.Count = 4; // Antialias level
	

	IDXGIAdapter* adapter = NULL; // Choose default graphics card
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE; // Require DX11 card
	HMODULE software = NULL; // Not using TYPE_SOFTWARE
	u32 flags = 0;
	D3D_FEATURE_LEVEL* pFeatureLevels = NULL;
	u32 featureLevels = 0;
	D3D_FEATURE_LEVEL* pFeatureLevel = NULL;

	int res = D3D11CreateDeviceAndSwapChain(
		adapter,
		driverType,
		software,
		flags,
		pFeatureLevels,
		featureLevels,
		D3D11_SDK_VERSION,
		&scd,
		&m_swapChain,
		&m_device,
		pFeatureLevel,
		&m_deviceContext);

	BE_ASSERT(res == 0);
}

void beRenderInterface::Impl::CreateBackBuffer()
{
	// Set back buffer as render target
	ID3D11Texture2D* backBufferTexture = NULL;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	m_device->CreateRenderTargetView(backBufferTexture, NULL, &m_backBuffer);
	m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, NULL);
	backBufferTexture->Release();
}

void beRenderInterface::Impl::InitialiseViewport(int width, int height)
{
	D3D11_VIEWPORT viewport = {0};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;

	m_deviceContext->RSSetViewports(1, &viewport);
}

void beRenderInterface::Deinit()
{
	BE_SAFE_RELEASE(m_impl->m_backBuffer);
	BE_SAFE_RELEASE(m_impl->m_swapChain);
	BE_SAFE_RELEASE(m_impl->m_device);
	BE_SAFE_RELEASE(m_impl->m_deviceContext);
}


void beRenderInterface::BeginFrame()
{
	m_impl->m_deviceContext->ClearRenderTargetView(m_impl->m_backBuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
}

void beRenderInterface::EndFrame()
{
	m_impl->m_swapChain->Present(0, 0);
}
