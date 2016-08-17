#include "bePCH.h"
#include "beShaderColour.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beDeferred.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <D3Dcompiler.h>

beShaderColour::beShaderColour()
	: m_pShader(nullptr)
	, m_vShader(nullptr)
{
}

beShaderColour::~beShaderColour()
{
	BE_ASSERT(!m_pShader);
	BE_ASSERT(!m_vShader);
}

bool beShaderColour::Init(beRenderInterface* ri, const beWString& pixelFilename, const beWString& vertexFilename)
{
	ID3D11Device* device = ri->GetDevice();

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
	}
	*/


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
	
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	D3D11_BUFFER_DESC matrixBufferDesc = {0};

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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


	bool success = m_matrixBuffer.Allocate(ri, sizeof(MatrixBufferType), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	if (!success)
	{
		return false;
	}

	return true;
}

void beShaderColour::Deinit()
{
	m_matrixBuffer.Release();
	BE_SAFE_RELEASE(m_layout);
	BE_SAFE_RELEASE(m_pShader);
	BE_SAFE_RELEASE(m_vShader);
}

void beShaderColour::SetShaderParameters(beRenderInterface* ri, const Matrix& viewMatrix)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	const Matrix& worldMatrix = ri->GetWorldMatrix();
	const Matrix& projectionMatrix = ri->GetProjectionMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedResource = {0};
	
	XMMATRIX xWM = XMLoadFloat4x4(&worldMatrix);
	XMMATRIX xVM = XMLoadFloat4x4(&viewMatrix);
	XMMATRIX xPM = XMLoadFloat4x4(&projectionMatrix);

	XMMATRIX txWorldMatrix = XMMatrixTranspose(xWM);
	XMMATRIX txViewMatrix = XMMatrixTranspose(xVM);
	XMMATRIX txProjectionMatrix = XMMatrixTranspose(xPM);

	auto dataPtr = (MatrixBufferType*)m_matrixBuffer.Map(ri);
	if (!dataPtr)
	{
		BE_ASSERT(false);
		return;
	}
	XMStoreFloat4x4(&dataPtr->world, txWorldMatrix);
	XMStoreFloat4x4(&dataPtr->view, txViewMatrix);
	XMStoreFloat4x4(&dataPtr->projection, txProjectionMatrix);

	m_matrixBuffer.Unmap(ri);

	ID3D11Buffer* constantBuffers[] = {m_matrixBuffer.GetBuffer()};
	unsigned int bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, constantBuffers);
}

void beShaderColour::Render(beRenderInterface* ri, int indexCount, int indexOffset)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vShader, nullptr, 0);
	deviceContext->PSSetShader(m_pShader, nullptr, 0);

	deviceContext->DrawIndexed(indexCount, 0, indexOffset);
}

void beShaderColour::RenderMore(beRenderInterface* ri, int indexCount, int indexOffset)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	deviceContext->DrawIndexed(indexCount, 0, indexOffset);
}
