#pragma once
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePimpl.h"
#include "beDebugWorldDecl.h"
import beMath;
import beRenderInterface;
import beAppData;
import beTexture;

class beShaderColour;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beDebugWorld
{
public:
	PIMPL_DECLARE(beDebugWorld);

	bool Init(beRenderInterface* ri);
	void Update(const beAppData& appData, const Matrix& viewMatrix);
	void Deinit();

	void SetRenderAxes(bool renderAxes);
	void ToggleRenderMouseRay();
	
	void Render(beRenderInterface* ri, beShaderColour* colourShader, const Matrix& viewMatrix, const Vec3& cameraPosition);
};
