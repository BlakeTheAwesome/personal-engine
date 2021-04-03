module;

#include "BlakesEngine/Core/beString.h"
#include <d3d11.h>

export module beDebugWorld;
import beMath;
import beRenderInterface;
import beMouse;
import beTexture;
import beShaderPack;
import beRenderBuffer;


export class beDebugWorld
{
public:
	~beDebugWorld();
	
	bool Init(beRenderInterface* ri);
	void Update(beRenderInterface& ri, const beMouse& mouse, const Matrix& viewMatrix);
	void Deinit();

	void SetRenderAxes(bool renderAxes);
	void ToggleRenderMouseRay();
	
	void Render(beRenderInterface* ri, beShaderColour* colourShader, const Matrix& viewMatrix, const Vec3& cameraPosition);

private:
	bool InitAxes(beRenderInterface* ri);

	beRenderBuffer m_axesVertexBuffer;
	beRenderBuffer m_axesIndexBuffer;
	beRenderBuffer m_mouseVertexBuffer;
	beRenderBuffer m_mouseIndexBuffer;

	beRenderBuffer m_gridVertexBuffer;
	beRenderBuffer m_gridLinesIndexBuffer;
	beRenderBuffer m_gridFilledIndexBuffer;
	bool m_renderAxes = false;
	bool m_renderMouse = false;
	bool m_haveMouseVerts = false;
};
