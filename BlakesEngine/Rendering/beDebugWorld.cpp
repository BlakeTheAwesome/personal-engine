#include "bePCH.h"
#include "beDebugWorld.h"

#include "BlakesEngine\Core\beAssert.h"
#include "BlakesEngine\Core\bePrintf.h"
#include "BlakesEngine\DataStructures\beVector.h"
#include "beRenderInterface.h"
#include "beRenderBuffer.h"
#include "BlakesEngine\Shaders/beShaderColour.h"

#include <d3d11.h>

#include <fstream>


struct VertexColourType
{
	Vec4 position;
	Vec4 colour;
};

PIMPL_DATA(beDebugWorld)
	bool InitAxes(beRenderInterface* ri);
	beRenderBuffer axesVertexBuffer;
	beRenderBuffer axesIndexBuffer;
	bool renderAxes;
PIMPL_DATA_END


PIMPL_CONSTRUCT(beDebugWorld)
	: renderAxes(false)
{
}

PIMPL_DESTROY(beDebugWorld)
{
	BE_ASSERT(!axesVertexBuffer.IsValid() && !axesIndexBuffer.IsValid());
}

bool beDebugWorld::Init(beRenderInterface* ri)
{
	bool success = true;
	success |= self.InitAxes(ri);
	return success;
}

bool beDebugWorld::Impl::InitAxes(beRenderInterface* ri)
{
	int vertexCount = 6;

	beVector<VertexColourType> vertices(vertexCount, vertexCount, 0);
	beVector<u32> indices(vertexCount, vertexCount, 0);
	
	// Load the vertex array with data.
	float AxisLength = 5.f;
	float w = 1.f;
	vertices[0].position = Vec4(0.f, 0.f, 0.f, w);
	vertices[0].colour = Vec4(1.f, 0.f, 0.f, 1.f);
	vertices[1].position = Vec4(AxisLength, 0.f, 0.f, w);
	vertices[1].colour = Vec4(1.f, 0.f, 0.f, 1.f);

	vertices[2].position = Vec4(0.f, 0.f, 0.f, w);
	vertices[2].colour = Vec4(0.f, 1.f, 0.f, 1.f);
	vertices[3].position = Vec4(0.f, AxisLength, 0.f, w);
	vertices[3].colour = Vec4(0.f, 1.f, 0.f, 1.f);

	vertices[4].position = Vec4(0.f, 0.f, 0.f, w);
	vertices[4].colour = Vec4(0.f, 0.f, 1.f, 1.f);
	vertices[5].position = Vec4(0.f, 0.f, AxisLength, w);
	vertices[5].colour = Vec4(0.f, 0.f, 1.f, 1.f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;


	bool success = axesVertexBuffer.Allocate(ri, decltype(vertices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	success = axesIndexBuffer.Allocate(ri, decltype(indices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, indices.begin());
	if (!success) { BE_ASSERT(false); return false; }
	
	return true;
}

void beDebugWorld::Deinit()
{
	self.axesIndexBuffer.Release();
	self.axesVertexBuffer.Release();
}

void beDebugWorld::Render(beRenderInterface* ri, beShaderColour* shaderColour)
{
	if (self.renderAxes)
	{
		ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
		unsigned int stride = self.axesVertexBuffer.ElementSize();
		unsigned int offset = 0;

		ID3D11Buffer* vertexBuffers[] = {self.axesVertexBuffer.GetBuffer()};
		deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
		deviceContext->IASetIndexBuffer(self.axesIndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		shaderColour->Render(ri, self.axesIndexBuffer.NumElements(), 0);
	}
}

void beDebugWorld::SetRenderAxes(bool v)
{
	self.renderAxes = v;
}
