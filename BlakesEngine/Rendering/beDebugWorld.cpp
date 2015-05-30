#include "bePCH.h"
#include "beDebugWorld.h"

#include "Core\beAssert.h"
#include "Core\bePrintf.h"
#include "DataStructures\beVector.h"
#include "beRenderInterface.h"
#include "Shaders/beShaderColour.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include <fstream>


struct VertexColourType
{
	XMFLOAT4 position;
	XMFLOAT4 colour;
};

PIMPL_DATA(beDebugWorld)
	bool InitAxes(beRenderInterface* ri);
	ID3D11Buffer* axesVertexBuffer;
	ID3D11Buffer* axesIndexBuffer;
	bool renderAxes;
PIMPL_DATA_END


PIMPL_CONSTRUCT(beDebugWorld)
	: axesVertexBuffer(nullptr)
	, axesIndexBuffer(nullptr)
	, renderAxes(false)
{
}

PIMPL_DESTROY(beDebugWorld)
{
	BE_ASSERT(!axesVertexBuffer && !axesIndexBuffer);
}

bool beDebugWorld::Init(beRenderInterface* ri)
{
	bool success = true;
	success |= self.InitAxes(ri);
	return success;
}

bool beDebugWorld::Impl::InitAxes(beRenderInterface* ri)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {0};
	D3D11_BUFFER_DESC indexBufferDesc = {0};
	D3D11_SUBRESOURCE_DATA vertexData = {0};
	D3D11_SUBRESOURCE_DATA indexData = {0};
	VertexColourType* vertices = nullptr;
	unsigned int* indices;
	HRESULT res;

	ID3D11Device* device = ri->GetDevice();

	int vertexCount = 6;

	vertices = new VertexColourType[vertexCount];
	indices = new unsigned int[vertexCount];
	
	// Load the vertex array with data.
	float AxisLength = 5.f;
	float w = 1.f;
	vertices[0].position = XMFLOAT4(0.f, 0.f, 0.f, w);
	vertices[0].colour = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
	vertices[1].position = XMFLOAT4(AxisLength, 0.f, 0.f, w);
	vertices[1].colour = XMFLOAT4(1.f, 0.f, 0.f, 1.f);

	vertices[2].position = XMFLOAT4(0.f, 0.f, 0.f, w);
	vertices[2].colour = XMFLOAT4(0.f, 1.f, 0.f, 1.f);
	vertices[3].position = XMFLOAT4(0.f, AxisLength, 0.f, w);
	vertices[3].colour = XMFLOAT4(0.f, 1.f, 0.f, 1.f);

	vertices[4].position = XMFLOAT4(0.f, 0.f, 0.f, w);
	vertices[4].colour = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
	vertices[5].position = XMFLOAT4(0.f, 0.f, AxisLength, w);
	vertices[5].colour = XMFLOAT4(0.f, 0.f, 1.f, 1.f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(decltype(*vertices)) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&vertexBufferDesc, &vertexData, &axesVertexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(decltype(*indices)) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&indexBufferDesc, &indexData, &axesIndexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	delete [] vertices;
	delete [] indices;

	return true;
}

void beDebugWorld::Deinit()
{
	BE_SAFE_RELEASE(self.axesIndexBuffer);
	BE_SAFE_RELEASE(self.axesVertexBuffer);
}

void beDebugWorld::Render(beRenderInterface* ri, beShaderColour* shaderColour)
{
	if (self.renderAxes)
	{
		ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
		unsigned int stride = sizeof(VertexColourType);
		unsigned int offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &self.axesVertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(self.axesIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		shaderColour->Render(ri, 6, 0);
	}
}

void beDebugWorld::SetRenderAxes(bool v)
{
	self.renderAxes = v;
}
