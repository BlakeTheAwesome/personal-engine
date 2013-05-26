#include "bePCH.h"
#include "beRenderInterface.h"

#include "Core/beMacros.h"

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

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device; // Mostly memory like stuff
	ID3D11DeviceContext* m_deviceContext; // Mostly GPUish stuff
};

BE_PIMPL_CPP_DECLARE(beRenderInterface)
{
	BE_PIMPL_CPP_BODY(beRenderInterface);
}

beRenderInterface::Impl::Impl()
	: m_swapChain(NULL)
	, m_device(NULL)
	, m_deviceContext(NULL)
{
}

beRenderInterface::Impl::~Impl()
{
}

void beRenderInterface::Init(void* pHWnd)
{
	HWND* hWnd = (HWND*)pHWnd;


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
		&m_impl->m_swapChain,
		&m_impl->m_device,
		pFeatureLevel,
		&m_impl->m_deviceContext);
}

void beRenderInterface::Deinit()
{
	BE_SAFE_RELEASE(m_impl->m_swapChain);
	BE_SAFE_RELEASE(m_impl->m_device);
	BE_SAFE_RELEASE(m_impl->m_deviceContext);
}
