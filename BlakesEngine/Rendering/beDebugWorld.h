#pragma once
#include "Core\beString.h"
#include "Core\bePimpl.h"
#include "Math\beMath.h"

class beRenderInterface;
class beTexture;
class beShaderColour;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beDebugWorld
{
public:
	PIMPL_DECLARE(beDebugWorld);

	bool Init(beRenderInterface* ri, bool renderAxes);
	void Deinit();
	
	void Render(beRenderInterface* ri, beShaderColour* colourShader);
};
