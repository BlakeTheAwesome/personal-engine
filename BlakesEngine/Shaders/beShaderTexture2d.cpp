#include "bePCH.h"
#include "beShaderTexture2d.h"
#include "Core\beAssert.h"
#include "Core\bePrintf.h"
#include "Core\beMacros.h"
#include "Rendering\beRenderInterface.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <D3Dcompiler.h>


struct MatrixBufferType
{
	Matrix ortho;
	Vec2 screenSize;
	Vec2 padding;
};

beShaderTexture2d::beShaderTexture2d()
	: m_pShader(nullptr)
	, m_vShader(nullptr)
	, m_sampleState(nullptr)
	, m_layout(nullptr)
	, m_matrixBuffer(nullptr)
{
}

beShaderTexture2d::~beShaderTexture2d()
{
	BE_ASSERT(!m_sampleState);
	BE_ASSERT(!m_pShader);
	BE_ASSERT(!m_vShader);
}

bool beShaderTexture2d::Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename)
{
	ID3D11Device* device = renderInterface->GetDevice();
	D3D11_SAMPLER_DESC samplerDesc;

	//ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vBuffer = nullptr;
	ID3D10Blob* pBuffer = nullptr;

	/*HRESULT res = D3DX11CompileFromFile(vertexFilename.c_str(), nullptr, nullptr, "Texture2dVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vBuffer, &errorMessage, nullptr);
	if (FAILED(res))
	{
		if (errorMessage)
		{
			bePRINTF("%s\n Filename:%s, res:0x%08x", errorMessage->GetBufferPointer(), vertexFilename.c_str(), res);
		}

		BE_ASSERT(false);
		return false;
	}

	res = D3DX11CompileFromFile(pixelFilename.c_str(), nullptr, nullptr, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pBuffer, &errorMessage, nullptr);
	if (FAILED(res))
	{
		if (errorMessage)
		{
			bePrintf::bePrintf(false, "", "%s\n Filename:%s, res:0x%08x", errorMessage->GetBufferPointer(), pixelFilename.c_str(), res);
		}

		BE_ASSERT(false);
		return false;
	}*/

	D3DReadFileToBlob(vertexFilename.c_str(), &vBuffer);
	D3DReadFileToBlob(pixelFilename.c_str(), &pBuffer);

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
	
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	D3D11_BUFFER_DESC bufferDesc = {0};

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
	
	unsigned int numElements = BE_ARRAY_DIMENSION(polygonLayout);

	res = device->CreateInputLayout(polygonLayout, numElements, vBuffer->GetBufferPointer(), vBuffer->GetBufferSize(), &m_layout);
	if (FAILED(res))
	{
		return false;
	}

	BE_SAFE_RELEASE(vBuffer);
	BE_SAFE_RELEASE(pBuffer);

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

void beShaderTexture2d::Deinit()
{
	BE_SAFE_RELEASE(m_sampleState);
	BE_SAFE_RELEASE(m_matrixBuffer);
	BE_SAFE_RELEASE(m_layout);
	BE_SAFE_RELEASE(m_pShader);
	BE_SAFE_RELEASE(m_vShader);
}

void beShaderTexture2d::SetShaderParameters(beRenderInterface* renderInterface, const Matrix& viewMatrix)
{
	ID3D11DeviceContext* deviceContext = renderInterface->GetDeviceContext();
	const Matrix& orthoMatrix = renderInterface->GetOrthoMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedResource = {0};
	
	// Lock the constant buffer so it can be written to.
	{
		XMMATRIX xOM = XMLoadFloat4x4(&orthoMatrix);
		XMMATRIX txOrthoMatrix = XMMatrixTranspose(xOM);

		HRESULT res = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(res))
		{
			return;
		}
	
		auto dataPtr = (MatrixBufferType*)mappedResource.pData;

		XMStoreFloat4x4(&dataPtr->ortho, xOM);
		dataPtr->screenSize = renderInterface->GetScreenSize();

		deviceContext->Unmap(m_matrixBuffer, 0);
	}
	
	unsigned int bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
}

void beShaderTexture2d::Render(beRenderInterface* renderInterface, int indexCount, ID3D11ShaderResourceView* texture)
{
	ID3D11DeviceContext* deviceContext = renderInterface->GetDeviceContext();
	
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vShader, nullptr, 0);
	deviceContext->PSSetShader(m_pShader, nullptr, 0);
	
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
