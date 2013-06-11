#pragma once
#include "core/bePimpl.h"
#include "core/beString.h"
#include "Math/beMath.h"

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct D3DXMATRIX;

class beWindow;

class beRenderInterface
{
public:
	BE_PIMPL_CREATE(beRenderInterface);

	void Init(beWindow* window, float nearPlane, float farPlane, bool vSync);
	void Deinit();

	void Update(float dt);

	void BeginFrame();
	void EndFrame();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	const Matrix& GetProjectionMatrix();
	const Matrix& GetWorldMatrix();
	const Matrix& GetOrthoMatrix();

	void GetVideoCardInfo(beString* name, unsigned int* memorySize);
};
