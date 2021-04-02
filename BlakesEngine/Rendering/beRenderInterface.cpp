#include "BlakesEngine/bePCH.h"
#include "beRenderInterface.h"

#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Window/beWindow.h"

#include <d3d11.h>
///#include <DirectXPackedVector.h>

import ZeroMem;
import beDirectXMath;
using namespace DirectXMath;

#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dx11.lib")
//#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "DirectXTK.lib")


// Note to self, going with a right hand coordinate system
// Y ( up )
// ^
// |
// |
// ---->X ( right )
// Z ( -forward ) pointing away from screen

// View matrix will look like (Right, Up, Forward, -Pos)
// [ Rx,  Ry,  Rz, 0]
// [ Ux.  Uy,  Uz, 0]
// [ Fx,  Fy,  Fz, 0]
// [-Px, -Py, -Pz, 1]

/*

mat4 LookAtRH( vec3 eye, vec3 target, vec3 up )
{
    vec3 zaxis = normal(eye - target);    // The "forward" vector.
    vec3 xaxis = normal(cross(up, zaxis));// The "right" vector.
    vec3 yaxis = cross(zaxis, xaxis);     // The "up" vector.
 
    // Create a 4x4 orientation matrix from the right, up, and forward vectors
    // This is transposed which is equivalent to performing an inverse 
    // if the matrix is orthonormalized (in this case, it is).
    mat4 orientation = {
       vec4( xaxis.x, yaxis.x, zaxis.x, 0 ),
       vec4( xaxis.y, yaxis.y, zaxis.y, 0 ),
       vec4( xaxis.z, yaxis.z, zaxis.z, 0 ),
       vec4(   0,       0,       0,     1 )
    };
     
    // Create a 4x4 translation matrix.
    // The eye position is negated which is equivalent
    // to the inverse of the translation matrix. 
    // T(v)^-1 == T(-v)
    mat4 translation = {
        vec4(   1,      0,      0,   0 ),
        vec4(   0,      1,      0,   0 ), 
        vec4(   0,      0,      1,   0 ),
        vec4(-eye.x, -eye.y, -eye.z, 1 )
    };
 
    // Combine the orientation and translation to compute 
    // the final view matrix. Note that the order of 
    // multiplication is reversed because the matrices
    // are already inverted.
    return ( orientation * translation );
}

mat4 LookAtRH_Fast( vec3 eye, vec3 target, vec3 up )
{
    vec3 zaxis = normal(eye - target);    // The "forward" vector.
    vec3 xaxis = normal(cross(up, zaxis));// The "right" vector.
    vec3 yaxis = cross(zaxis, xaxis);     // The "up" vector.
 
    // Create a 4x4 view matrix from the right, up, forward and eye position vectors
    mat4 viewMatrix = {
        vec4(      xaxis.x,            yaxis.x,            zaxis.x,       0 ),
        vec4(      xaxis.y,            yaxis.y,            zaxis.y,       0 ),
        vec4(      xaxis.z,            yaxis.z,            zaxis.z,       0 ),
        vec4(-dot( xaxis, eye ), -dot( yaxis, eye ), -dot( zaxis, eye ),  1 )
    };
     
    return viewMatrix;
}*/




PIMPL_DATA(beRenderInterface)
	void CreateDevice(HWND* hWnd, int windowWidth, int windowHeight, int clientWidth, int clientHeight);
	void CreateDepthBuffer(int clientWidth, int clientHeight);
	void CreateStencilView();
	void CreateBlendStates();
	void CreateBackBuffer();
	void CreateRasterState();
	void InitialiseViewport(float clientWidth, float clientHeight);
	void CreateMatrices(float width, float height, float nearPlane, float farPlane);

	Matrix m_projectionMatrix;
	Matrix m_worldMatrix;
	Matrix m_orthoMatrix;
	Matrix m_orthoMatrixPixelCoord;

	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr; // Mostly memory like stuff
	ID3D11DeviceContext* m_deviceContext = nullptr; // Mostly GPUish stuff
	ID3D11RenderTargetView *m_backBuffer = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilState* m_depthDisabledStencilState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;
	ID3D11RasterizerState* m_wireframeRasterState = nullptr;
	ID3D11BlendState* m_alphaEnableBlendingState = nullptr;
	ID3D11BlendState* m_alphaDisableBlendingState = nullptr;

	Vec3 m_lightDirection {0.f, 0.f, 0.f};
	float m_width = 0.f;
	float m_height = 0.f;
	float m_near = 0.f;
	float m_far = 0.f;
	size_t m_videoCardMemory = 0;
	char m_videoCardDescription[128];
	bool m_vsync_enabled = false;
	bool m_wireframe = false;

PIMPL_DATA_END

PIMPL_CONSTRUCT(beRenderInterface)
{
	m_videoCardDescription[0] = '\0';
}

PIMPL_DESTROY(beRenderInterface)
{
	BE_ASSERT(!m_device);
}

void beRenderInterface::Init(beWindow* window, float nearPlane, float farPlane, bool vSync)
{
	self.m_vsync_enabled = vSync;

	HWND* hWnd = (HWND*)window->GetHWnd();
	int width = window->GetWidth();
	int height = window->GetHeight();
	float fWidth = (float)width;
	float fHeight = (float)height;

	self.m_width = fWidth;
	self.m_height = fHeight;
	self.m_near = nearPlane;
	self.m_far = farPlane;
	self.CreateDevice(hWnd, window->GetWindowWidth(), window->GetWindowHeight(), width, height);
	self.CreateDepthBuffer(width, height);
	self.CreateStencilView();
	self.CreateBlendStates();
	self.CreateBackBuffer();
	self.CreateRasterState();
	self.InitialiseViewport(fWidth, fHeight);
	self.CreateMatrices(fWidth, fHeight, nearPlane, farPlane);
}

void beRenderInterface::Deinit()
{
	self.m_swapChain->SetFullscreenState(FALSE, nullptr); // Need to be in windowed mode to close cleanly
	
	BE_SAFE_RELEASE(self.m_alphaEnableBlendingState);
	BE_SAFE_RELEASE(self.m_alphaDisableBlendingState);
	BE_SAFE_RELEASE(self.m_backBuffer);
	BE_SAFE_RELEASE(self.m_swapChain);
	BE_SAFE_RELEASE(self.m_wireframeRasterState);
	BE_SAFE_RELEASE(self.m_rasterState);
	BE_SAFE_RELEASE(self.m_depthStencilView);
	BE_SAFE_RELEASE(self.m_depthStencilState);
	BE_SAFE_RELEASE(self.m_depthDisabledStencilState);
	BE_SAFE_RELEASE(self.m_depthStencilBuffer);
	BE_SAFE_RELEASE(self.m_device);
	BE_SAFE_RELEASE(self.m_deviceContext);
}

void beRenderInterface::Impl::CreateDevice(HWND* hWnd, int windowWidth, int windowHeight, int clientWidth, int clientHeight)
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
		defer({ factory->Release(); });

		res = factory->EnumAdapters(0, &adapter);
		//res = factory->EnumAdapters(1, &adapter);
		if(FAILED(res)) { BE_ASSERT(false); return; }
		defer({ adapter->Release(); });
		
		DXGI_ADAPTER_DESC adapterDesc; ZeroMem(&adapterDesc);
		res = adapter->GetDesc(&adapterDesc);
		if(FAILED(res)) { BE_ASSERT(false); return; }

		m_videoCardMemory = adapterDesc.DedicatedVideoMemory;
		size_t stringLength = 0;
		res = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);

		res = adapter->EnumOutputs(0, &adapterOutput);
		if(FAILED(res)) { BE_ASSERT(false); return; }
		defer({ adapterOutput->Release(); });

		
		unsigned int numModes = 0;
		res = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
		if(FAILED(res)) { BE_ASSERT(false); return; }

		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		defer({ delete [] displayModeList; });

		res = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if(FAILED(res)) { BE_ASSERT(false); return; }

		bool found = false;
		for (unsigned int i = 0; i < numModes; i++)
		{
			if(displayModeList[i].Width == (unsigned int)windowWidth)
			{
				if(displayModeList[i].Height == (unsigned int)windowHeight)
				{
					refreshRateNumerator = displayModeList[i].RefreshRate.Numerator;
					refreshRateDenominator = displayModeList[i].RefreshRate.Denominator;
					found = true;
					break;
				}
			}
		}

		if (!found)
		{
			BE_ASSERT(found); // #TODO: This is all wrong, doesn't work on all monitors, needs to be better. Can push past assert for now.
			refreshRateNumerator = displayModeList[numModes-1].RefreshRate.Numerator;
			refreshRateDenominator = displayModeList[numModes-1].RefreshRate.Denominator;
		}
	}


	DXGI_SWAP_CHAIN_DESC scd; ZeroMem(&scd);
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = clientWidth;
	scd.BufferDesc.Height = clientHeight;
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
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	
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
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	{
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
		depthStencilDesc.DepthEnable = false;
		HRESULT res = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthDisabledStencilState);
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

void beRenderInterface::Impl::CreateBlendStates()
{
	D3D11_BLEND_DESC blendDesc{};
	
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	HRESULT res = m_device->CreateBlendState(&blendDesc, &m_alphaEnableBlendingState);
	if(FAILED(res)) { BE_ASSERT(false); return; }

	blendDesc.RenderTarget[0].BlendEnable = false;
	// Create the blend state using the description.
	res = m_device->CreateBlendState(&blendDesc, &m_alphaDisableBlendingState);
	if(FAILED(res)) { BE_ASSERT(false); return; }
}

void beRenderInterface::Impl::CreateBackBuffer()
{
	// Set back buffer as render target
	ID3D11Texture2D* backBufferTexture = nullptr;
	HRESULT res = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(res))
	{
		LOG("ERROR res:0x%08x", res);
		BE_ASSERT(false);
	}
	//LOG("m_swapChain->GetBuffer, res = 0x%08x, backBufferTexture = 0x%08x", res, backBufferTexture);
	res = m_device->CreateRenderTargetView(backBufferTexture, nullptr, &m_backBuffer);
	if (FAILED(res))
	{
		LOG("ERROR res:0x%08x", res);
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

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	res = m_device->CreateRasterizerState(&rasterDesc, &m_wireframeRasterState);
	if(FAILED(res)) { BE_ASSERT(false); return; }

	m_deviceContext->RSSetState(m_rasterState);
}

void beRenderInterface::Impl::InitialiseViewport(float width, float height)
{
	// top left is -1,-1
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = width;
	viewport.Height = height;

	m_deviceContext->RSSetViewports(1, &viewport);
}

void beRenderInterface::Impl::CreateMatrices(float width, float height, float nearPlane, float farPlane)
{
	float fov = (float)PI / 4.0f;
	float screenAspect = width / height;
	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovRH(fov, screenAspect, nearPlane, farPlane);
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX orthoMatrix = XMMatrixOrthographicRH(width, height, nearPlane, farPlane);
	XMVECTOR negHalfScreenVec = XMVectorSet(-width/2.f, -height/2.f, 0.f, 0.f);
	XMMATRIX orthoMatrixPixelSpace = XMMatrixMultiply(orthoMatrix, XMMatrixTranslationFromVector(negHalfScreenVec));


	XMStoreFloat4x4(&m_projectionMatrix, projectionMatrix);
	XMStoreFloat4x4(&m_worldMatrix, worldMatrix);
	XMStoreFloat4x4(&m_orthoMatrix, orthoMatrix);
	XMStoreFloat4x4(&m_orthoMatrixPixelCoord, orthoMatrixPixelSpace);
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

void beRenderInterface::EnableAlpha()
{
	float blendFactor[4] = {0.f, 0.f, 0.f, 0.f};
	self.m_deviceContext->OMSetBlendState(self.m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void beRenderInterface::DisableAlpha()
{
	float blendFactor[4] = {0.f, 0.f, 0.f, 0.f};
	self.m_deviceContext->OMSetBlendState(self.m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

Vec2 beRenderInterface::GetScreenSize() const
{
	return Vec2(self.m_width, self.m_height);
}

void beRenderInterface::ToggleWireframe()
{
	self.m_wireframe = !self.m_wireframe;
	if (self.m_wireframe)
	{
		self.m_deviceContext->RSSetState(self.m_wireframeRasterState);
	}
	else
	{
		self.m_deviceContext->RSSetState(self.m_rasterState);
	}
}

void beRenderInterface::SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView, float width, float height, float nearPlane, float farPlane)
{
	self.m_deviceContext->OMSetRenderTargets(1, &renderTarget, depthStencilView);
	self.CreateMatrices(width, height, nearPlane, farPlane);
	self.InitialiseViewport(width, height);
}

void beRenderInterface::RestoreRenderTarget()
{
	self.m_deviceContext->OMSetRenderTargets(1, &self.m_backBuffer, self.m_depthStencilView);
	self.CreateMatrices(self.m_width, self.m_height, self.m_near, self.m_far);
	self.InitialiseViewport(self.m_width, self.m_height);
}

void beRenderInterface::ClearDepth()
{
	self.m_deviceContext->ClearDepthStencilView(self.m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
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

const Matrix& beRenderInterface::GetOrthoMatrixPixelCoord() const
{
	return self.m_orthoMatrixPixelCoord;
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

ID3D11DepthStencilView* beRenderInterface::GetDepthStencilView()
{
	return self.m_depthStencilView;
}

void beRenderInterface::GetVideoCardInfo(beString* cardName, size_t* memory)
{
	cardName->assign(self.m_videoCardDescription);
	*memory = self.m_videoCardMemory;
	return;
}

