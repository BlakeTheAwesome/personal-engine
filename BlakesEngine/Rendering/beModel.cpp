#include "bePCH.h"
#include "beModel.h"

#include "Core\beAssert.h"
#include "Rendering\beRenderInterface.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

struct VertexType
{
	XMFLOAT3A position;
	XMFLOAT4A color;
};

beModel::beModel()
	: m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
	, m_vertexCount(0)
	, m_indexCount(0)
{
}

beModel::~beModel()
{
	BE_ASSERT(!m_vertexBuffer && !m_indexBuffer);
}

bool beModel::Init(beRenderInterface* ri)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {0};
	D3D11_BUFFER_DESC indexBufferDesc = {0};
	D3D11_SUBRESOURCE_DATA vertexData = {0};
	D3D11_SUBRESOURCE_DATA indexData = {0};
	VertexType* vertices = NULL;
	unsigned int* indices;
	HRESULT res;

	ID3D11Device* device = ri->GetDevice();

	m_vertexCount = 3;
	m_indexCount = 3;

	vertices = new VertexType[m_vertexCount];
	indices = new unsigned int[m_indexCount];
	
	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3A(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3A(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3A(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);

	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	delete [] vertices;
	delete [] indices;

	return true;
}

void beModel::Deinit()
{
	BE_SAFE_RELEASE(m_indexBuffer);
	BE_SAFE_RELEASE(m_vertexBuffer);
}

void beModel::Render(beRenderInterface* ri)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int beModel::GetIndexCount()
{
	return m_indexCount;
}
