#pragma once
#include "core/bePimpl.h"
#include "core/beString.h"

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

	const D3DXMATRIX& GetProjectionMatrix();
	const D3DXMATRIX& GetWorldMatrix();
	const D3DXMATRIX& GetOrthoMatrix();

	void GetVideoCardInfo(beString* name, unsigned int* memorySize);
};
