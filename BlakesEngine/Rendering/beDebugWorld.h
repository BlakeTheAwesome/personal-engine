#pragma once
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePimpl.h"
import beMath;

class beRenderInterface;
class beTexture;
class beShaderColour;
struct beAppData;
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
