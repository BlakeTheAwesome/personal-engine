#include "bePCH.h"
#include "beShaderLitTexture.h"
#include "Core\beAssert.h"
#include "Core\bePrintf.h"
#include "Core\beMacros.h"
#include "Core\beDeferred.h"
#include "Rendering\beRenderInterface.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <D3Dcompiler.h>

struct CameraBufferType
{
	Vec3 pos;
	float padding;
};

struct MatrixBufferType
{
	Matrix world;
	Matrix view;
	Matrix projection;
};

struct LightBufferType
{
	Vec4 ambientColour;
	Vec4 diffuseColour;
	Vec3 lightDirection;
    float specularPower;
    Vec4 specularColor;
};

beShaderLitTexture::beShaderLitTexture()
	: m_pShader(nullptr)
	, m_vShader(nullptr)
	, m_sampleState(nullptr)
	, m_layout(nullptr)
	, m_matrixBuffer(nullptr)
	, m_lightBuffer(nullptr)
	, m_cameraBuffer(nullptr)
{
}

beShaderLitTexture::~beShaderLitTexture()
{
	BE_ASSERT(!m_sampleState);
	BE_ASSERT(!m_pShader);
	BE_ASSERT(!m_vShader);
}

bool beShaderLitTexture::Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename)
{
	ID3D11Device* device = renderInterface->GetDevice();
	D3D11_SAMPLER_DESC samplerDesc;

	//ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vBuffer = nullptr;
	ID3D10Blob* pBuffer = nullptr;

	/*HRESULT res = D3DCompileFromFile(vertexFilename.c_str(), nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vBuffer, &errorMessage);
	if (FAILED(res))
	{
		if (errorMessage)
		{
			bePRINTF("%s\n Filename:%s, res:0x%08x", errorMessage->GetBufferPointer(), vertexFilename.c_str(), res);
		}

		BE_ASSERT(false);
		return false;
	}

	res = D3DCompileFromFile(pixelFilename.c_str(), nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pBuffer, &errorMessage);
	if (FAILED(res))
	{
		if (errorMessage)
		{
			bePRINTF("%s\n Filename:%s, res:0x%08x", errorMessage->GetBufferPointer(), pixelFilename.c_str(), res);
		}

		BE_ASSERT(false);
		return false;
	}*/

	D3DReadFileToBlob(vertexFilename.c_str(), &vBuffer);
	D3DReadFileToBlob(pixelFilename.c_str(), &pBuffer);
	DeferredCall d1([&vBuffer] {BE_SAFE_RELEASE(vBuffer);});
	DeferredCall d2([&pBuffer] {BE_SAFE_RELEASE(pBuffer);});

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
	D3D11_BUFFER_DESC bufferDesc = {0};

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
	
	unsigned int numElements = BE_ARRAY_DIMENSION(polygonLayout);

	res = device->CreateInputLayout(polygonLayout, numElements, vBuffer->GetBufferPointer(), vBuffer->GetBufferSize(), &m_layout);
	if (FAILED(res))
	{
		return false;
	}

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(MatrixBufferType);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	res = device->CreateBuffer(&bufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(res))
	{
		return false;
	}
	
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(LightBufferType);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	res = device->CreateBuffer(&bufferDesc, nullptr, &m_lightBuffer);
	if (FAILED(res))
	{
		return false;
	}
	
	
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(CameraBufferType);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	res = device->CreateBuffer(&bufferDesc, nullptr, &m_cameraBuffer);
	if (FAILED(res))
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
	BE_SAFE_RELEASE(m_sampleState);
	BE_SAFE_RELEASE(m_cameraBuffer);
	BE_SAFE_RELEASE(m_lightBuffer);
	BE_SAFE_RELEASE(m_matrixBuffer);
	BE_SAFE_RELEASE(m_layout);
	BE_SAFE_RELEASE(m_pShader);
	BE_SAFE_RELEASE(m_vShader);
}

void beShaderLitTexture::SetShaderParameters(beRenderInterface* renderInterface, const Matrix& viewMatrix, const Vec3& cameraPosition)
{
	ID3D11DeviceContext* deviceContext = renderInterface->GetDeviceContext();
	const Matrix& worldMatrix = renderInterface->GetWorldMatrix();
	const Matrix& projectionMatrix = renderInterface->GetProjectionMatrix();
	const Vec3& lightDirection = renderInterface->GetLightDirection();

	D3D11_MAPPED_SUBRESOURCE mappedResource = {0};
	
	// Lock the constant buffer so it can be written to.
	{
		XMMATRIX xWM = XMLoadFloat4x4(&worldMatrix);
		XMMATRIX xVM = XMLoadFloat4x4(&viewMatrix);
		XMMATRIX xPM = XMLoadFloat4x4(&projectionMatrix);

		XMMATRIX txWorldMatrix = XMMatrixTranspose(xWM);
		XMMATRIX txViewMatrix = XMMatrixTranspose(xVM);
		XMMATRIX txProjectionMatrix = XMMatrixTranspose(xPM);

		HRESULT res = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(res))
		{
			return;
		}
	
		auto dataPtr = (MatrixBufferType*)mappedResource.pData;

		XMStoreFloat4x4(&dataPtr->world, txWorldMatrix);
		XMStoreFloat4x4(&dataPtr->view, txViewMatrix);
		XMStoreFloat4x4(&dataPtr->projection, txProjectionMatrix);

		deviceContext->Unmap(m_matrixBuffer, 0);
	}
	{
		HRESULT res = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(res))
		{
			return;
		}
	
		auto dataPtr = (LightBufferType*)mappedResource.pData;
		dataPtr->diffuseColour = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
		dataPtr->ambientColour = Vec4(0.0f, 0.1f, 0.3f, 1.0f);
		dataPtr->lightDirection = lightDirection;

		
		dataPtr->specularPower = 32.f;
		dataPtr->specularColor = Vec4(1.f, 1.f, 1.f, 1.f);
		
		deviceContext->Unmap(m_lightBuffer, 0);
	}
	{
		HRESULT res = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(res))
		{
			return;
		}
	
		auto dataPtr = (CameraBufferType*)mappedResource.pData;
		dataPtr->pos = cameraPosition;
		dataPtr->padding = 0.f;
		
		deviceContext->Unmap(m_cameraBuffer, 0);
	}

	unsigned int bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
}

void beShaderLitTexture::Render(beRenderInterface* renderInterface, int indexCount, ID3D11ShaderResourceView* texture)
{
	ID3D11DeviceContext* deviceContext = renderInterface->GetDeviceContext();
	
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vShader, nullptr, 0);
	deviceContext->PSSetShader(m_pShader, nullptr, 0);
	
	deviceContext->PSSetShaderResources(0, 1, &texture);
	//deviceContext->VSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
