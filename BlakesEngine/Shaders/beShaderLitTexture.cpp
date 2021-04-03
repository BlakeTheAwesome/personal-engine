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

module beShaderPack:litTex;

import beRenderInterface;
import beModel;

using CameraBufferType = beShaderDefinitions::CameraBuffer;

beShaderLitTexture::~beShaderLitTexture()
{
	BE_ASSERT(!m_sampleState);
	BE_ASSERT(!m_pShader);
	BE_ASSERT(!m_vShader);
}

bool beShaderLitTexture::Init(beRenderInterface* ri, const beWString& pixelFilename, const beWString& vertexFilename)
{
	ID3D11Device* device = ri->GetDevice();
	D3D11_SAMPLER_DESC samplerDesc;

	ID3D10Blob* vBuffer = nullptr;
	ID3D10Blob* pBuffer = nullptr;

	D3DReadFileToBlob(vertexFilename.c_str(), &vBuffer);
	D3DReadFileToBlob(pixelFilename.c_str(), &pBuffer);
	defer({BE_SAFE_RELEASE(vBuffer);});
	defer({BE_SAFE_RELEASE(pBuffer);});

	HRESULT res = device->CreateVertexShader(vBuffer->GetBufferPointer(), vBuffer->GetBufferSize(), nullptr, &m_vShader);
	if (FAILED(res))
	{
		return false;
	}

	res = device->CreatePixelShader(pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), nullptr, &m_pShader);
	if (FAILED(res))
	{
		return false;
	}
	
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;
	
	u32 numElements = (u32)std::size(polygonLayout);

	res = device->CreateInputLayout(polygonLayout, numElements, vBuffer->GetBufferPointer(), vBuffer->GetBufferSize(), &m_layout);
	if (FAILED(res))
	{
		return false;
	}

	bool success = m_lightBuffer.Allocate(ri, sizeof(ShaderParams), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	if (!success)
	{
		return false;
	}

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
	res = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(res))
	{
		BE_ASSERT(false);
		return false;
	}


	return true;
}

void beShaderLitTexture::Deinit()
{
	m_lightBuffer.Release();
	BE_SAFE_RELEASE(m_sampleState);
	BE_SAFE_RELEASE(m_layout);
	BE_SAFE_RELEASE(m_pShader);
	BE_SAFE_RELEASE(m_vShader);
}

beShaderLitTexture::ShaderParams beShaderLitTexture::GetDefaultShaderParams(beRenderInterface* ri)
{
	beShaderLitTexture::ShaderParams shaderParams;
	shaderParams.diffuseColour = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
	shaderParams.ambientColour = Vec4(0.0f, 0.1f, 0.3f, 1.0f);
	shaderParams.specularColour = Vec4(1.f, 1.f, 1.f, 1.f);
	shaderParams.specularPower = 32.f;
	
	const Vec3& lightDirection = ri->GetLightDirection();
	shaderParams.lightDirection = lightDirection;
	
	return shaderParams;
}

void beShaderLitTexture::SetShaderParameters(beRenderInterface* ri, const ShaderParams& shaderParams)
{
	bool update = false;
	update = update || (shaderParams.ambientColour != m_lastShaderParams.ambientColour);
	update = update || (shaderParams.diffuseColour != m_lastShaderParams.diffuseColour);
	update = update || (shaderParams.lightDirection != m_lastShaderParams.lightDirection);
	update = update || (shaderParams.specularPower != m_lastShaderParams.specularPower);
	update = update || (shaderParams.specularColour != m_lastShaderParams.specularColour);

	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	
	if (update)
	{
		m_lastShaderParams = shaderParams;
		auto dataPtr = (ShaderParams*)m_lightBuffer.Map(ri);
		if (!dataPtr)
		{
			return;
		}
		*dataPtr = shaderParams;
		m_lightBuffer.Unmap(ri);
	}

	ID3D11Buffer* psConstantBuffers[] = { m_lightBuffer.GetBuffer() };
	deviceContext->PSSetConstantBuffers(CBuffers::LightBuffer, 1, psConstantBuffers);
}

void beShaderLitTexture::Render(beRenderInterface* ri, int indexCount, ID3D11ShaderResourceView* texture)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void beShaderLitTexture::SetActive(beRenderInterface* ri)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();

	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vShader, nullptr, 0);
	deviceContext->PSSetShader(m_pShader, nullptr, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
}
