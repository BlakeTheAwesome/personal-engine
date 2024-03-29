module;
#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Platform/beWindows.h"
#include "beShaderCBufferDefinitions.h"
#include "BlakesEngine/Core/beString.h"

#include <windowsx.h>
#include <d3d11.h>
#include <D3Dcompiler.h>

module beShaderPack:tex2d;
import beArray;
import RangeIter;
import beRenderInterface;
import beContainerHelpers;

enum
{
	c_quadVertexCount = 6,
	c_quadIndexCount = 6,
};

beShaderTexture2d::~beShaderTexture2d()
{
	BE_ASSERT(!m_clampedSampleState);
	BE_ASSERT(!m_wrappedSampleState);
	BE_ASSERT(!m_pShader);
	BE_ASSERT(!m_vShaderNormalised);
	BE_ASSERT(!m_vShaderPixelSpace);
}

bool beShaderTexture2d::Init(beRenderInterface* ri, const beWString& pixelFilename, const beWString& vertexFilenameNormalised, const beWString& vertexFilenamePixelSpace)
{
	ID3D11Device* device = ri->GetDevice();
	D3D11_SAMPLER_DESC samplerDesc;

	//ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vBufferNorm = nullptr;
	ID3D10Blob* vBufferPix = nullptr;
	ID3D10Blob* pBuffer = nullptr;

	D3DReadFileToBlob(vertexFilenameNormalised.c_str(), &vBufferNorm);
	D3DReadFileToBlob(vertexFilenamePixelSpace.c_str(), &vBufferPix);
	D3DReadFileToBlob(pixelFilename.c_str(), &pBuffer);
	defer({BE_SAFE_RELEASE(vBufferNorm);});
	defer({BE_SAFE_RELEASE(vBufferPix);});
	defer({ BE_SAFE_RELEASE(pBuffer);});


	HRESULT res = device->CreateVertexShader(vBufferNorm->GetBufferPointer(), vBufferNorm->GetBufferSize(), nullptr, &m_vShaderNormalised);
	if (FAILED(res)) { return false; }

	res = device->CreateVertexShader(vBufferPix->GetBufferPointer(), vBufferPix->GetBufferSize(), nullptr, &m_vShaderPixelSpace);
	if (FAILED(res)) { return false; }

	res = device->CreatePixelShader(pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), nullptr, &m_pShader);
	if (FAILED(res)) { return false; }
	

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	//D3D11_BUFFER_DESC bufferDesc{};

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	
	u32 numElements = (u32)std::size(polygonLayout);

	res = device->CreateInputLayout(polygonLayout, numElements, vBufferNorm->GetBufferPointer(), vBufferNorm->GetBufferSize(), &m_layout);
	if (FAILED(res)) { return false; }
	
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	res = device->CreateSamplerState(&samplerDesc, &m_wrappedSampleState);
	if (FAILED(res)) { return false; }

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	res = device->CreateSamplerState(&samplerDesc, &m_clampedSampleState);
	if (FAILED(res)) { return false; }





	// Copied from beBitmap - buffers for RenderQuad
	beArray<VertexType, c_quadVertexCount> vertices;
	beArray<u32, c_quadIndexCount> indices;

	for (auto i : RangeIter((u32)indices.size()))
	{
		indices[i] = i;
	}

	bool success = m_vertexBuffer.Allocate(ri, ElementSize(vertices), (u32)vertices.size(), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, vertices.data());
	if (!success) { BE_ASSERT(false); return false; }

	success = m_indexBuffer.Allocate(ri, ElementSize(indices), (u32)indices.size(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, beRendering::Topology::TriangleList, 0, 0, indices.data());
	if (!success) { BE_ASSERT(false); return false; }

	success = m_positionBuffer.Allocate(ri, sizeof(beShaderDefinitions::PositionBuffer), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	if (!success) { BE_ASSERT(false); return false; }

	return true;
}

void beShaderTexture2d::Deinit()
{
	m_positionBuffer.Release();
	m_indexBuffer.Release();
	m_vertexBuffer.Release();
	BE_SAFE_RELEASE(m_clampedSampleState);
	BE_SAFE_RELEASE(m_wrappedSampleState);
	BE_SAFE_RELEASE(m_layout);
	BE_SAFE_RELEASE(m_pShader);
	BE_SAFE_RELEASE(m_vShaderNormalised);
	BE_SAFE_RELEASE(m_vShaderPixelSpace);
}

void beShaderTexture2d::SetShaderParameters(beRenderInterface* ri, const Matrix& viewMatrix)
{
}

void beShaderTexture2d::RenderQuad(beRenderInterface* ri, Vec2 uvMin, Vec2 uvMax, bool scaleToScreen, ID3D11ShaderResourceView* texture, bool normalisedVerts, TextureMode textureMode)
{
	{
		Vec2 size = uvMax-uvMin;
		if (scaleToScreen)
		{
			size *= ri->GetScreenSize();
		}

		auto vertices = (beArray<VertexType, c_quadVertexCount>*)m_vertexBuffer.Map(ri);
		vertices->at(0).position = Vec2(0.f, size.y);  // TL
		vertices->at(0).uv = Vec2(0.f, 0.f);
		vertices->at(1).position = Vec2(size.x, size.y);  // TR
		vertices->at(1).uv = Vec2(1.f, 0.f);
		vertices->at(2).position = Vec2(size.x, 0.f);  // BR
		vertices->at(2).uv = Vec2(1.f, 1.f);
		vertices->at(3).position = Vec2(size.x, 0.f);  // BR
		vertices->at(3).uv = Vec2(1.f, 1.f);
		vertices->at(4).position = Vec2(0.f, 0.f);  // BL.
		vertices->at(4).uv = Vec2(0.f, 1.f);
		vertices->at(5).position = Vec2(0.f, size.y);  // TL.
		vertices->at(5).uv = Vec2(0.f, 0.f);

		m_vertexBuffer.Unmap(ri);
	}

	{
		auto* dataPtr = (beShaderDefinitions::PositionBuffer*)m_positionBuffer.Map(ri);
		if (!dataPtr) { BE_ASSERT(false); return; }
		dataPtr->positionOffset = uvMin;
		dataPtr->colour = V41();
		m_positionBuffer.Unmap(ri);
	}

	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	ID3D11Buffer* vertexBuffers[] ={m_vertexBuffer.GetBuffer()};
	ID3D11Buffer* constantBuffers[] ={m_positionBuffer.GetBuffer()};
	u32 strides[] ={(u32)m_vertexBuffer.ElementSize()};
	u32 offsets[] ={0};
	
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_indexBuffer.D3DIndexTopology());
	deviceContext->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->VSSetConstantBuffers(CBuffers::PositionBuffer, 1, constantBuffers);

	ri->DisableZBuffer();
	Render(ri, c_quadIndexCount, texture, normalisedVerts, textureMode);
	ri->RestoreRenderTarget();
}

void beShaderTexture2d::Render(beRenderInterface* ri, int indexCount, ID3D11ShaderResourceView* texture, bool normalisedVerts, TextureMode textureMode)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	
	deviceContext->IASetInputLayout(m_layout);

	if (normalisedVerts)
	{
		deviceContext->VSSetShader(m_vShaderNormalised, nullptr, 0);
	}
	else
	{
		deviceContext->VSSetShader(m_vShaderPixelSpace, nullptr, 0);
	}

	deviceContext->PSSetShader(m_pShader, nullptr, 0);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	switch (textureMode)
	{
		case TextureMode::Wrapped: deviceContext->PSSetSamplers(0, 1, &m_wrappedSampleState); break;
		case TextureMode::Clamped: deviceContext->PSSetSamplers(0, 1, &m_clampedSampleState); break;
		default: BE_ASSERT(false);
	}
	
		
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
