#include "bePCH.h"
#include "beRenderInterface.h"

#include "Core/beMacros.h"
#include "Core/beAssert.h"
#include "Core/bePrintf.h"

#include "Window/beWindow.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
///#include <DirectXPackedVector.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dx11.lib")
//#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "DirectXTK.lib")

PIMPL_DATA(beRenderInterface)
	void CreateDevice(HWND* hWnd, int width, int height);
	void CreateDepthBuffer(int width, int height);
	void CreateStencilView();
	void CreateBackBuffer();
	void CreateRasterState();
	void InitialiseViewport(int width, int height);
	void CreateMatrices(int width, int height, float nearPlane, float farPlane);

	Matrix m_projectionMatrix;
	Matrix m_worldMatrix;
	Matrix m_orthoMatrix;

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device; // Mostly memory like stuff
	ID3D11DeviceContext* m_deviceContext; // Mostly GPUish stuff
	ID3D11RenderTargetView *m_backBuffer;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	float m_width;
	float m_height;
	unsigned int m_videoCardMemory;
	char m_videoCardDescription[128];
	bool m_vsync_enabled;

	Vec3 m_lightDirection;
PIMPL_DATA_END

PIMPL_CONSTRUCT(beRenderInterface)
	: m_swapChain(nullptr)
	, m_device(nullptr)
	, m_deviceContext(nullptr)
	, m_backBuffer(nullptr)
	
	, m_vsync_enabled(false)
	, m_videoCardMemory(0)
	, m_depthStencilBuffer(nullptr)
	, m_depthDisabledStencilState(nullptr)
	, m_depthStencilState(nullptr)
	, m_depthStencilView(nullptr)
	, m_rasterState(nullptr)
	, m_lightDirection(0.f, 0.f, 0.f)
	, m_width(0.f)
	, m_height(0.f)
{
	m_videoCardDescription[0] = '\0';
}

PIMPL_DESTROY(beRenderInterface)
{
}

void beRenderInterface::Init(beWindow* window, float nearPlane, float farPlane, bool vSync)
{
	self.m_vsync_enabled = vSync;

	HWND* hWnd = (HWND*)window->GetHWnd();
	int width = window->GetWidth();
	int height = window->GetHeight();
	self.m_width = (float)width;
	self.m_height = (float)height;
	self.CreateDevice(hWnd, width, height);
	self.CreateDepthBuffer(width, height);
	self.CreateStencilView();
	self.CreateBackBuffer();
	self.CreateRasterState();
	self.InitialiseViewport(width, height);
	self.CreateMatrices(width, height, nearPlane, farPlane);
}

void beRenderInterface::Impl::CreateDevice(HWND* hWnd, int width, int height)
{
	unsigned int refreshRateNumerator = 0;
	unsigned int refreshRateDenominator = 0;

	// Enumerate devices
	{
		
		IDXGIFactory* factory = nullptr;
		IDXGIAdapter* adapter = nullptr;
		IDXGIOutput* adapterOutput = nullptr;
		
		HRESULT res = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if(FAILED(res)) { BE_ASSERT(false); return; }

		res = factory->EnumAdapters(0, &adapter);
		//res = factory->EnumAdapters(1, &adapter);
		if(FAILED(res)) { BE_ASSERT(false); return; }
		
		DXGI_ADAPTER_DESC adapterDesc = {0};
		res = adapter->GetDesc(&adapterDesc);
		if(FAILED(res)) { BE_ASSERT(false); return; }

		m_videoCardMemory = adapterDesc.DedicatedVideoMemory;
		size_t stringLength = 0;
		res = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);

		res = adapter->EnumOutputs(0, &adapterOutput);
		if(FAILED(res)) { BE_ASSERT(false); return; }

		
		unsigned int numModes;
		res = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
		if(FAILED(res)) { BE_ASSERT(false); return; }
		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		res = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if(FAILED(res)) { BE_ASSERT(false); return; }

		bool found = false;
		for (unsigned int i = 0; i < numModes; i++)
		{
			if(displayModeList[i].Width == (unsigned int)width)
			{
				if(displayModeList[i].Height == (unsigned int)height)
				{
					refreshRateNumerator = displayModeList[i].RefreshRate.Numerator;
					refreshRateDenominator = displayModeList[i].RefreshRate.Denominator;
					found = true;
					break;
				}
			}
		}
		BE_ASSERT(found);
		delete [] displayModeList;

		BE_SAFE_RELEASE(adapterOutput);
		BE_SAFE_RELEASE(adapter);
		BE_SAFE_RELEASE(factory);
	}


	DXGI_SWAP_CHAIN_DESC scd = {0};
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = *hWnd;
	scd.Windowed = TRUE;
	scd.SampleDesc.Count = 1; // Antialias level
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching

	if (m_vsync_enabled)
	{
		scd.BufferDesc.RefreshRate.Numerator = refreshRateNumerator;
		scd.BufferDesc.RefreshRate.Denominator = refreshRateDenominator;
	}
	else
	{
		scd.BufferDesc.RefreshRate.Numerator = 0;
		scd.BufferDesc.RefreshRate.Denominator = 1;
	}
	

	D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;

	IDXGIAdapter* adapter = nullptr; // Choose default graphics card
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE; // Require DX11 card
	HMODULE software = nullptr; // Not using TYPE_SOFTWARE

	u32 flags = 0;
	D3D_FEATURE_LEVEL* pFeatureLevels = &fl;
	u32 featureLevels = 1;
	D3D_FEATURE_LEVEL* pFeatureLevel = nullptr;
	
	#if defined(_DEBUG)  
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

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

void beRenderInterface::Impl::CreateDepthBuffer(int width, int height)
{
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
	
	HRESULT res = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return; }
}

void beRenderInterface::Impl::CreateStencilView()
{
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {0};

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state.
		HRESULT res = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(res)) { BE_ASSERT(false); return; }
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	}

	// State with depth disabled for toggling Z buffer
	{
		D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc = {0};

		// Set up the description of the stencil state.
		depthDisabledStencilDesc.DepthEnable = false;
		depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthDisabledStencilDesc.StencilEnable = true;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state.
		HRESULT res = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
		if (FAILED(res)) { BE_ASSERT(false); return; }
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HRESULT res = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(res)) { BE_ASSERT(false); return; }
}

void beRenderInterface::Impl::CreateBackBuffer()
{
	// Set back buffer as render target
	ID3D11Texture2D* backBufferTexture = nullptr;
	HRESULT res = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(res))
	{
		bePRINTF("ERROR res:0x%08x", res);
		BE_ASSERT(false);
	}
	//bePRINTF("m_swapChain->GetBuffer, res = 0x%08x, backBufferTexture = 0x%08x", res, backBufferTexture);
	res = m_device->CreateRenderTargetView(backBufferTexture, nullptr, &m_backBuffer);
	if (FAILED(res))
	{
		bePRINTF("ERROR res:0x%08x", res);
		BE_ASSERT(false);
	}

	m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, m_depthStencilView);
	backBufferTexture->Release();
}

void beRenderInterface::Impl::CreateRasterState()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	memset(&rasterDesc, 0, sizeof(rasterDesc));

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	HRESULT res = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(res)) { BE_ASSERT(false); return; }

	m_deviceContext->RSSetState(m_rasterState);
}

void beRenderInterface::Impl::InitialiseViewport(int width, int height)
{
	// top left is -1,-1
	D3D11_VIEWPORT viewport = {0};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 0.0f;
	viewport.Width = (float)width;
	viewport.Height = (float)height;

	m_deviceContext->RSSetViewports(1, &viewport);
}

void beRenderInterface::Impl::CreateMatrices(int width, int height, float nearPlane, float farPlane)
{
	float fov = (float)PI / 4.0f;
	float screenAspect = (float)width / (float)height;
	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fov, screenAspect, nearPlane, farPlane);
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX orthoMatrix = XMMatrixOrthographicLH((float)width, (float)height, nearPlane, farPlane);

	XMStoreFloat4x4(&m_projectionMatrix, projectionMatrix);
	XMStoreFloat4x4(&m_worldMatrix, worldMatrix);
	XMStoreFloat4x4(&m_orthoMatrix, orthoMatrix);
}

void beRenderInterface::Deinit()
{
	self.m_swapChain->SetFullscreenState(FALSE, nullptr); // Need to be in windowed mode to close cleanly

	BE_SAFE_RELEASE(self.m_backBuffer);
	BE_SAFE_RELEASE(self.m_swapChain);
	BE_SAFE_RELEASE(self.m_rasterState);
	BE_SAFE_RELEASE(self.m_depthStencilView);
	BE_SAFE_RELEASE(self.m_depthStencilState);
	BE_SAFE_RELEASE(self.m_depthDisabledStencilState);
	BE_SAFE_RELEASE(self.m_depthStencilBuffer);
	BE_SAFE_RELEASE(self.m_device);
	BE_SAFE_RELEASE(self.m_deviceContext);
}

static float s_offset = 0.0f;

void beRenderInterface::BeginFrame()
{
	//float r = sinf(0.0f + s_offset);
	//float g = sinf(0.2f + s_offset);
	//float b = sinf(0.4f + s_offset);
	//self.m_deviceContext->ClearRenderTargetView(self.m_backBuffer, D3DXCOLOR(r, g, b, 1.0f));

	float colour[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	self.m_deviceContext->ClearRenderTargetView(self.m_backBuffer, colour);
	self.m_deviceContext->ClearDepthStencilView(self.m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void beRenderInterface::Update(float dt)
{
	s_offset += dt;
	XMVECTOR lightDir = XMVectorSet(sinf(s_offset* 2.f), sinf(s_offset * 0.7f), cosf(s_offset* 2.f), 0.f);
	XMVECTOR normalisedDir = XMVector3Normalize(lightDir);
	XMStoreFloat3(&self.m_lightDirection, normalisedDir);
	//self.m_lightDirection = Vec3(0.f, 0.f, 1.f);
}

void beRenderInterface::EndFrame()
{
	if (self.m_vsync_enabled)
	{
		self.m_swapChain->Present(1, 0);
	}
	else
	{
		self.m_swapChain->Present(0, 0);
	}
}


void beRenderInterface::EnableZBuffer()
{
	self.m_deviceContext->OMSetDepthStencilState(self.m_depthStencilState, 1);
}

void beRenderInterface::DisableZBuffer()
{
	self.m_deviceContext->OMSetDepthStencilState(self.m_depthDisabledStencilState, 1);
}

Vec2 beRenderInterface::GetScreenSize()
{
	return Vec2(self.m_width, self.m_height);
}

const Matrix& beRenderInterface::GetProjectionMatrix() const
{
	return self.m_projectionMatrix;
}

const Matrix& beRenderInterface::GetWorldMatrix() const
{
	return self.m_worldMatrix;
}

const Matrix& beRenderInterface::GetOrthoMatrix() const
{
	return self.m_orthoMatrix;
}

const Vec3& beRenderInterface::GetLightDirection() const
{
	return self.m_lightDirection;
}

ID3D11Device* beRenderInterface::GetDevice()
{
	return self.m_device;
}

ID3D11DeviceContext* beRenderInterface::GetDeviceContext()
{
	return self.m_deviceContext;
}

void beRenderInterface::GetVideoCardInfo(beString* cardName, unsigned int* memory)
{
	cardName->assign(self.m_videoCardDescription);
	*memory = self.m_videoCardMemory;
	return;
}

