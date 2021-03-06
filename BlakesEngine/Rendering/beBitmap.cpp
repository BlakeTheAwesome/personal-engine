#include "BlakesEngine/bePCH.h"
#include "beBitmap.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/DataStructures/beArray.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Rendering/beTexture.h"

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

beBitmap::~beBitmap()
{
	BE_ASSERT(!m_vertexBuffer.IsValid() && !m_indexBuffer.IsValid());
}


bool beBitmap::Init(beRenderInterface* ri, const beTexture& texture)
{
	const float width = (float)texture.GetWidth();
	const float height = (float)texture.GetHeight();
	if (!InitCommon(ri, width, height))
	{
		return false;
	}
	m_texture.Set(texture);
	return true;
}

bool beBitmap::Init(beRenderInterface* ri, beShaderPack* shaderPack, float width, float height, const beStringView& textureFilename)
{
	if (!InitCommon(ri, width, height))
	{
		return false;
	}
	return m_texture.Init(ri, shaderPack, textureFilename, {});
}

bool beBitmap::InitCommon(beRenderInterface* ri, float width, float height)
{
	m_size = Vec2(width, height);

	enum {
		vertexCount = 6,
		indexCount = 6,
	};
	beArray<VertexType, vertexCount> vertices;
	beArray<u32, indexCount> indices;

	// Load the vertex array with data.
	//height = 1.f; width = 1.f;
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

	auto success = m_vertexBuffer.Allocate(ri, ElementSize(vertices), vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.data());
	if(!success) { BE_ASSERT(false); return false; }

	success = m_indexBuffer.Allocate(ri, ElementSize(indices), indexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 0, indices.data());
	if(!success) { BE_ASSERT(false); return false; }

	success = m_positionBuffer.Allocate(ri, sizeof(PositionBufferType), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	if(!success) { BE_ASSERT(false); return false; }
	
	return true;
}

void beBitmap::Deinit()
{
	m_texture.Deinit();
	m_positionBuffer.Release();
	m_indexBuffer.Release();
	m_vertexBuffer.Release();
}

#include "beFont.h"
bool beBitmap::InitText(beRenderInterface* ri, const beFont* font, const beStringView& string, float scale, float maxWidth, u32 invalidStringCharacter, bool fixedWidth, beFont::WrapMode wrapMode)
{
	beFont::StringInfo info;
	if (!font->CreateString(ri, string, scale, maxWidth, invalidStringCharacter, fixedWidth, wrapMode, &info))
	{
		return false;
	}
	m_vertexBuffer = std::move(info.vertexBuffer);
	m_indexBuffer = std::move(info.indexBuffer);
	m_texture.Set(*font->GetTexture());
	
	const auto success = m_positionBuffer.Allocate(ri, sizeof(PositionBufferType), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	if(!success) { BE_ASSERT(false); return false; }

	return true;
}

bool beBitmap::LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename)
{
	return m_texture.Init(ri, shaderPack, textureFilename, {});
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

void beBitmap::SetPosition(const Vec2& v)
{
	m_position = v;
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
	BE_ASSERT(m_positionBuffer.IsValid());
	BE_ASSERT(m_vertexBuffer.IsValid());
	BE_ASSERT(m_indexBuffer.IsValid());

	if (m_dirtyPositionBuffer)
	{
		m_dirtyPositionBuffer = false;

		//Vec2 windowSize = ri->GetScreenSize();
		const float xOffset = m_position.x + (m_anchorPoint.x * 1.f);// - (windowSize.x / 2.f);
		const float yOffset = m_position.y + (m_anchorPoint.y * 1.f);// - (windowSize.y / 2.f);

		PositionBufferType* dataPtr = (PositionBufferType*)m_positionBuffer.Map(ri);
		if (!dataPtr)
		{
			return;
		}
		dataPtr->positionOffset = Vec2(xOffset, yOffset);
		dataPtr->colour = m_colour;
		m_positionBuffer.Unmap(ri);
	}

	ID3D11Buffer* vertexBuffers[] = {m_vertexBuffer.GetBuffer()};
	ID3D11Buffer* constantBuffers[] = {m_positionBuffer.GetBuffer()};
	const u32 strides[] = {(u32)m_vertexBuffer.ElementSize()};
	const u32 offsets[] = {0};

	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_indexBuffer.D3DIndexTopology());
	deviceContext->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->VSSetConstantBuffers(CBUFIDX_PositionBuffer, 1, constantBuffers);
}

ID3D11ShaderResourceView* beBitmap::GetTexture() const
{
	return m_texture.GetTexture();
}

int beBitmap::GetIndexCount()
{
	return m_indexBuffer.NumElements();
}

