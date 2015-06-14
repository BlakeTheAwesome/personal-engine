#include "bePCH.h"
#include "beBitmap.h"

#include "Core\beAssert.h"
#include "Core\bePrintf.h"
#include "DataStructures\beVector.h"
#include "Rendering\beRenderInterface.h"
#include "Rendering\beTexture.h"

#include <d3d11.h>

#include <fstream>

struct PositionBufferType
{
	Vec4 colour;
	Vec2 positionOffset;
	Vec2 padding;
};

struct VertexType
{
	Vec2 position;
	Vec2 texCoord;
};

beBitmap::beBitmap()
	: m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
	, m_positionBuffer(nullptr)
	, m_vertexCount(0)
	, m_indexCount(0)
	, m_dirtyPositionBuffer(true)
	, m_size(0.f, 0.f)
	, m_position(0.f, 0.f)
	, m_anchorPoint(0.f, 0.f)
	, m_colour(1.f, 1.f, 1.f, 1.f)
{
	m_texture = new beTexture();
}

beBitmap::~beBitmap()
{
	BE_ASSERT(!m_vertexBuffer && !m_indexBuffer);
	BE_SAFE_DELETE(m_texture);
}


bool beBitmap::Init(beRenderInterface* ri, const beTexture& texture)
{
	float width = (float)texture.GetWidth();
	float height = (float)texture.GetHeight();
	if (!InitCommon(ri, width, height))
	{
		return false;
	}
	m_texture->Set(texture);
	return true;
}

bool beBitmap::Init(beRenderInterface* ri, float width, float height, const beWString& textureFilename)
{
	if (!InitCommon(ri, width, height))
	{
		return false;
	}
	return m_texture->Init(ri, textureFilename);
}

bool beBitmap::InitCommon(beRenderInterface* ri, float width, float height)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {0};
	D3D11_BUFFER_DESC indexBufferDesc = {0};
	D3D11_BUFFER_DESC positionBufferDesc = {0};
	D3D11_SUBRESOURCE_DATA vertexData = {0};
	D3D11_SUBRESOURCE_DATA indexData = {0};
	VertexType* vertices = nullptr;
	unsigned int* indices;
	HRESULT res;

	ID3D11Device* device = ri->GetDevice();

	m_size = Vec2(width, height);
	m_vertexCount = 6;
	m_indexCount = 6;

	vertices = new VertexType[m_vertexCount];
	indices = new unsigned int[m_indexCount];



	//height = 1.f; width = 1.f;



	// Load the vertex array with data.

	vertices[0].position = Vec2(0.f, height);  // TL
	vertices[0].texCoord = Vec2(0.f, 0.f);

	vertices[1].position = Vec2(width, height);  // TR
	vertices[1].texCoord = Vec2(1.f, 0.f);

	vertices[2].position = Vec2(width, 0.f);  // BR
	vertices[2].texCoord = Vec2(1.f, 1.f);

	vertices[3].position = Vec2(width, 0.f);  // BR
	vertices[3].texCoord = Vec2(1.f, 1.f);

	vertices[4].position = Vec2(0.f, 0.f);  // BL.
	vertices[4].texCoord = Vec2(0.f, 1.f);

	vertices[5].position = Vec2(0.f, height);  // TL.
	vertices[5].texCoord = Vec2(0.f, 0.f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(decltype(*vertices)) * m_vertexCount;
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
	indexBufferDesc.ByteWidth = sizeof(decltype(*indices)) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }


	positionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	positionBufferDesc.ByteWidth = sizeof(PositionBufferType);
	positionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	positionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	positionBufferDesc.MiscFlags = 0;
	positionBufferDesc.StructureByteStride = 0;

	res = device->CreateBuffer(&positionBufferDesc, nullptr, &m_positionBuffer);
	if (FAILED(res))
	{
		return false;
	}

	delete [] vertices;
	delete [] indices;
	return true;
}

void beBitmap::Deinit()
{
	m_texture->Deinit();
	BE_SAFE_RELEASE(m_positionBuffer);
	BE_SAFE_RELEASE(m_indexBuffer);
	BE_SAFE_RELEASE(m_vertexBuffer);
}

#include "beFont.h"
bool beBitmap::InitText(beRenderInterface* ri, const beFont* font, const beString& string, float maxWidth, u32 invalidStringCharacter)
{
	beFont::StringInfo info = {0};
	if (!font->CreateString(ri, string, maxWidth, invalidStringCharacter, &info))
	{
		return false;
	}
	m_vertexBuffer = info.vertexBuffer;
	m_indexBuffer = info.indexBuffer;
	m_vertexCount = info.vertexCount;
	m_indexCount = info.vertexCount;
	m_texture->Set(*font->GetTexture());
	
	D3D11_BUFFER_DESC positionBufferDesc = {0};

	positionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	positionBufferDesc.ByteWidth = sizeof(PositionBufferType);
	positionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	positionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	positionBufferDesc.MiscFlags = 0;
	positionBufferDesc.StructureByteStride = 0;

	ID3D11Device* device = ri->GetDevice();
	HRESULT res = device->CreateBuffer(&positionBufferDesc, nullptr, &m_positionBuffer);
	if (FAILED(res))
	{
		return false;
	}

	return true;
}

bool beBitmap::LoadTexture(beRenderInterface* ri, const beWString& textureFilename)
{
	return m_texture->Init(ri, textureFilename);
}

void beBitmap::SetColour(const Vec4 & colour)
{
	m_colour = colour;
	m_dirtyPositionBuffer = true;
}

Vec2 beBitmap::GetPosition() const
{
	return m_position;
}

void beBitmap::SetPosition(float x, float y)
{
	m_position = Vec2(x, y);
	m_dirtyPositionBuffer = true;
}

void beBitmap::SetAnchorPoint(float x, float y)
{
	m_anchorPoint = Vec2(x, y);
	m_dirtyPositionBuffer = true;
}

void beBitmap::Render(beRenderInterface* ri)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	BE_ASSERT(m_positionBuffer);
	BE_ASSERT(m_vertexBuffer);
	BE_ASSERT(m_indexBuffer);

	if (m_dirtyPositionBuffer)
	{
		m_dirtyPositionBuffer = false;

		//Vec2 windowSize = ri->GetScreenSize();
		float xOffset = m_position.x + (m_anchorPoint.x * 1.f);// - (windowSize.x / 2.f);
		float yOffset = m_position.y + (m_anchorPoint.y * 1.f);// - (windowSize.y / 2.f);

		D3D11_MAPPED_SUBRESOURCE mappedResource = {0};
		HRESULT res = deviceContext->Map(m_positionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(res))
		{
			return;
		}
		auto dataPtr = (PositionBufferType*)mappedResource.pData;
		dataPtr->positionOffset = Vec2(xOffset, yOffset);
		dataPtr->colour = m_colour;
		deviceContext->Unmap(m_positionBuffer, 0);
	}



	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	unsigned int bufferNumber = 1; // 0 is matrix buffer set in SetShaderParameters
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_positionBuffer);
}

ID3D11ShaderResourceView * beBitmap::GetTexture() const
{
	return m_texture->GetTexture();
}

int beBitmap::GetIndexCount()
{
	return m_indexCount;
}

