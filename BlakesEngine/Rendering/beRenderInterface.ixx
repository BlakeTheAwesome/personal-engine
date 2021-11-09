module;

#include "BlakesEngine/Core/beString.h"
#include <d3d11.h>

export module beRenderInterface;

import beMath;
import beWindow;

export class beRenderInterface
{
public:
	beRenderInterface();
	~beRenderInterface();

	void Init(beWindow* window, float nearPlane, float farPlane, bool vSync);
	void Deinit();

	void Update(float dt);

	void BeginFrame();
	void EndFrame();

	void EnableZBuffer();
	void DisableZBuffer();

	void EnableAlpha();
	void DisableAlpha();

	Vec2 GetScreenSize() const;

	void ToggleWireframe();

	void SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView, float height, float width, float nearPlane, float farPlane);
	void RestoreRenderTarget();
	void ClearDepth();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11DepthStencilView* GetDepthStencilView();

	const Matrix& GetProjectionMatrix() const;
	const Matrix& GetWorldMatrix() const;
	const Matrix& GetOrthoMatrix() const;
	const Matrix& GetOrthoMatrixPixelCoord() const;
	const Vec3& GetLightDirection() const;

	void GetVideoCardInfo(beString* name, size_t* memorySize);

	private:

	void CreateDevice(HWND hWnd, int windowWidth, int windowHeight, int clientWidth, int clientHeight);
	void CreateDepthBuffer(int clientWidth, int clientHeight);
	void CreateStencilView();
	void CreateBlendStates();
	void CreateBackBuffer();
	void CreateRasterState();
	void InitialiseViewport(float clientWidth, float clientHeight);
	void CreateMatrices(float width, float height, float nearPlane, float farPlane);

	Matrix m_projectionMatrix = IdentityMatrix();
	Matrix m_worldMatrix = IdentityMatrix();
	Matrix m_orthoMatrix = IdentityMatrix();
	Matrix m_orthoMatrixPixelCoord = IdentityMatrix();

	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr; // Mostly memory like stuff
	ID3D11DeviceContext* m_deviceContext = nullptr; // Mostly GPUish stuff
	ID3D11RenderTargetView* m_backBuffer = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilState* m_depthDisabledStencilState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;
	ID3D11RasterizerState* m_wireframeRasterState = nullptr;
	ID3D11BlendState* m_alphaEnableBlendingState = nullptr;
	ID3D11BlendState* m_alphaDisableBlendingState = nullptr;

	Vec3 m_lightDirection{0.f, 0.f, 0.f};
	float m_width = 0.f;
	float m_height = 0.f;
	float m_near = 0.f;
	float m_far = 0.f;
	size_t m_videoCardMemory = 0;
	char m_videoCardDescription[128];
	bool m_vsync_enabled = false;
	bool m_wireframe = false;
};
