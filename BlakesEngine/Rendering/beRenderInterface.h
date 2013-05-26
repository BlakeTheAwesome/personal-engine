#pragma once
#include "core/bePimpl.h"

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;

class beWindow;

class beRenderInterface
{
public:
	BE_PIMPL_CREATE(beRenderInterface);

	void Init(beWindow* window);
	void Deinit();

	void BeginFrame();
	void EndFrame();
};
