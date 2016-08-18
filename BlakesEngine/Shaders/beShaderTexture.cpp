#include "bePCH.h"
#include "beShaderTexture.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <D3Dcompiler.h>

struct MatrixBufferType
{
	Matrix world;
	Matrix view;
	Matrix projection;
};

struct ColourBufferType
{
	Vec4 colour;
};

beShaderTexture::~beShaderTexture()
{
	BE_ASSERT(!m_matrixBuffer.IsValid());
	BE_ASSERT(!m_colourBuffer.IsValid());
	BE_ASSERT(!m_sampleState);
	BE_ASSERT(!m_pShader);
	BE_ASSERT(!m_vShader);
}

bool beShaderTexture::Init(beRenderInterface* ri, const beWString& pixelFilename, const beWString& vertexFilename)
{
	ID3D11Device* device = ri->GetDevice();
	D3D11_SAMPLER_DESC samplerDesc;

	//ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vBuffer = nullptr;
	ID3D10Blob* pBuffer = nullptr;

	//HRESULT res = D3DCompileFromFile(vertexFilename.c_str(), nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vBuffer, &errorMessage);
	//if (FAILED(res))
	//{
	//	if (errorMessage)
	//	{
	//		bePRINTF("%s\n Filename:%s, res:0x%08x", errorMessage->GetBufferPointer(), vertexFilename.c_str(), res);
	//	}

	//	BE_ASSERT(false);
	//	return false;
	//}

	//res = D3DCompileFromFile(pixelFilename.c_str(), nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pBuffer, &errorMessage);
	//if (FAILED(res))
	//{
	//	if (errorMessage)
	//	{
	//		bePrintf::bePrintf(false, "", "%s\n Filename:%s, res:0x%08x", errorMessage->GetBufferPointer(), pixelFilename.c_str(), res);
	//	}

	//	BE_ASSERT(false);
	//	return false;
	//}

	D3DReadFileToBlob(vertexFilename.c_str(), &vBuffer);
	D3DReadFileToBlob(pixelFilename.c_str(), &pBuffer);
	DeferredCall d1([&vBuffer] {BE_SAFE_RELEASE(vBuffer);});
	DeferredCall d2([&pBuffer] {BE_SAFE_RELEASE(pBuffer);});

	HRESULT	res = device->CreateVertexShader(vBuffer->GetBufferPointer(), vBuffer->GetBufferSize(), nullptr, &m_vShader);
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
	
	bool success = m_matrixBuffer.Allocate(ri, sizeof(MatrixBufferType), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER,  D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	if (!success) { return false; }

	success = m_colourBuffer.Allocate(ri, sizeof(ColourBufferType), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER,  D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	if (!success) { return false; }

	
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

void beShaderTexture::Deinit()
{
	m_colourBuffer.Release();
	m_matrixBuffer.Release();
	BE_SAFE_RELEASE(m_sampleState);
	BE_SAFE_RELEASE(m_layout);
	BE_SAFE_RELEASE(m_pShader);
	BE_SAFE_RELEASE(m_vShader);
}

void beShaderTexture::SetColour(const Vec4 & colour)
{
	m_colour = colour;
	m_colourDirty = true;
}

void beShaderTexture::SetShaderParameters(beRenderInterface* ri, const Matrix& viewMatrix)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	const Matrix& worldMatrix = ri->GetWorldMatrix();
	const Matrix& projectionMatrix = ri->GetProjectionMatrix();

	{
		XMMATRIX xWM = XMLoadFloat4x4(&worldMatrix);
		XMMATRIX xVM = XMLoadFloat4x4(&viewMatrix);
		XMMATRIX xPM = XMLoadFloat4x4(&projectionMatrix);

		XMMATRIX txWorldMatrix = XMMatrixTranspose(xWM);
		XMMATRIX txViewMatrix = XMMatrixTranspose(xVM);
		XMMATRIX txProjectionMatrix = XMMatrixTranspose(xPM);

		auto dataPtr = (MatrixBufferType*)m_matrixBuffer.Map(ri);
		if (!dataPtr)
		{
			return;
		}

		XMStoreFloat4x4(&dataPtr->world, txWorldMatrix);
		XMStoreFloat4x4(&dataPtr->view, txViewMatrix);
		XMStoreFloat4x4(&dataPtr->projection, txProjectionMatrix);

		m_matrixBuffer.Unmap(ri);
	}

	if (m_colourDirty)
	{
		auto dataPtr = (ColourBufferType*)m_colourBuffer.Map(ri);
		if (!dataPtr)
		{
			return;
		}
		dataPtr->colour = m_colour;
		m_colourBuffer.Unmap(ri);
		m_colourDirty = false;
	}
	
	ID3D11Buffer* buffers[] = {m_matrixBuffer.GetBuffer(), m_colourBuffer.GetBuffer()};
	deviceContext->VSSetConstantBuffers(0, 2, buffers);
}

void beShaderTexture::Render(beRenderInterface* ri, int indexCount, ID3D11ShaderResourceView* texture)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vShader, nullptr, 0);
	deviceContext->PSSetShader(m_pShader, nullptr, 0);
	
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
