#pragma once
#include "BlakesEngine\Core/bePimpl.h"
#include "BlakesEngine\Core/beString.h"
#include "BlakesEngine\Math\beMath.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

class beWindow;

class beRenderInterface
{
public:
	PIMPL_DECLARE(beRenderInterface);

	void Init(beWindow* window, float nearPlane, float farPlane, bool vSync);
	void Deinit();

	void Update(float dt);

	void BeginFrame();
	void EndFrame();

	void EnableZBuffer();
	void DisableZBuffer();

	void EnableAlpha();
	void DisableAlpha();

	Vec2 GetScreenSize();

	void ToggleWireframe();

	void SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView, float height, float width, float nearPlane, float farPlane);
	void RestoreRenderTarget();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11DepthStencilView* GetDepthStencilView();

	const Matrix& GetProjectionMatrix() const;
	const Matrix& GetWorldMatrix() const;
	const Matrix& GetOrthoMatrix() const;
	const Vec3& GetLightDirection() const;

	void GetVideoCardInfo(beString* name, unsigned int* memorySize);
};
