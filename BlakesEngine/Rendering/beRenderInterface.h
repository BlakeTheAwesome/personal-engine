#pragma once
#include "core/bePimpl.h"

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;

class beRenderInterface
{
public:
	BE_PIMPL_CREATE(beRenderInterface);

	void Init(void* hWnd);
	void Deinit();
};
