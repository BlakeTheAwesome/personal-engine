#include "BlakesEngine/bePCH.h"
#include "beRenderBuffer.h"
#include "beRenderInterface.h"

#include "BlakesEngine/Core/beAssert.h"

#include <d3d11.h>

bool beRenderBuffer::Allocate(beRenderInterface* ri, int elementSize, int numElements, int d3dUsage, u32 d3dBindFlags, beRendering::Topology topology, u32 d3dCPUAccessFlags, u32 d3dMiscFlags, void* initialData)
{
	return Allocate(ri, elementSize, numElements, d3dUsage, d3dBindFlags, beRendering::GetD3DTopology(topology), d3dCPUAccessFlags, d3dMiscFlags, initialData);
}
bool beRenderBuffer::Allocate(beRenderInterface* ri, int elementSize, int numElements, int d3dUsage, u32 d3dBindFlags, int d3dIndexTopology, u32 d3dCPUAccessFlags, u32 d3dMiscFlags, void* initialData)
{
	BE_ASSERT(elementSize > 0);
	BE_ASSERT(numElements > 0);
	BE_ASSERT((d3dBindFlags == D3D11_BIND_INDEX_BUFFER) == (d3dIndexTopology != 0));

	int newBufferSize = elementSize * numElements;
	if (m_buffer != nullptr)
	{
		BE_ASSERT(m_d3dUsage == d3dUsage);
		BE_ASSERT(m_d3dBindFlags == d3dBindFlags);
		BE_ASSERT(m_d3dCPUAccessFlags == d3dCPUAccessFlags);
		BE_ASSERT(m_d3dMiscFlags == d3dMiscFlags);

		if (m_bufferSize < newBufferSize)
		{
			BE_SAFE_RELEASE(m_buffer);
		}
	}
	

	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205132%28v=vs.85%29.aspx
	//In terms of Direct3D 10, default resources can be accessed directly by the GPU, dynamic and staging resources can be directly accessed by the CPU.
	//There are two main kinds of resources: mappable and non-mappable. Resources created with dynamic or staging usages are mappable, while resources created with default or immutable usages are non-mappable.
	//The CPU can only read from resources created with the D3D10_USAGE_STAGING flag. Since resources created with this flag cannot be set as outputs of the pipeline, if the CPU wants to read the data in a resource generated by the GPU, the data must be copied to a resource created with the staging flag.
	//ID3D10Device::CopyResource and ID3D10Device::CopySubresourceRegion are asynchronous; the copy has not necessarily executed by the time the method returns. The benefit of this is that the application does not pay the performance cost of actually copying the data until the CPU accesses the data, which is when Map is called.
	//  When using ID3D10Device::CopyResource and ID3D10Device::CopySubresourceRegion, you want to wait 2 frames before accessing the data

	if (!m_buffer)
	{
		
		D3D11_SUBRESOURCE_DATA subResData = {0};
		subResData.pSysMem = initialData;
		subResData.SysMemPitch = 0;
		subResData.SysMemSlicePitch = 0;

		D3D11_BUFFER_DESC bufferDesc = {0};	
		bufferDesc.Usage = (D3D11_USAGE)d3dUsage;
		bufferDesc.ByteWidth = newBufferSize;
		bufferDesc.BindFlags = d3dBindFlags;
		bufferDesc.CPUAccessFlags = d3dCPUAccessFlags;
		bufferDesc.MiscFlags = d3dMiscFlags;
		bufferDesc.StructureByteStride = elementSize;

		HRESULT res = ri->GetDevice()->CreateBuffer(&bufferDesc, initialData ? &subResData : nullptr, &m_buffer);
		if(FAILED(res)) { BE_ASSERT(false); return false; }
		
		m_d3dUsage = d3dUsage;
		m_d3dBindFlags = d3dBindFlags;
		m_d3dCPUAccessFlags = d3dCPUAccessFlags;
		m_d3dMiscFlags = d3dMiscFlags;
	}
	else if (initialData)
	{
		Update(ri, initialData, newBufferSize);
	}
	
	m_bufferSize = newBufferSize;
	m_numElements = numElements;
	m_elementSize = elementSize;
	m_d3dIndexTopology = d3dIndexTopology;

	return true;
}

void beRenderBuffer::Update(beRenderInterface* ri, void* data, int dataLen)
{
	BE_ASSERT(dataLen <= m_bufferSize);
	auto deviceContext = ri->GetDeviceContext();

	switch (m_d3dUsage)
	{
		case D3D11_USAGE_DEFAULT:
		{
			D3D11_BOX box = {0};
			box.left = 0;
			box.right = dataLen;
			box.top = 0;
			box.bottom = 1;
			box.front = 0;
			box.back = 1;

			deviceContext->UpdateSubresource(m_buffer, 0, &box, data, 0, 0);
		}
		break;

		case D3D11_USAGE_STAGING:
		case D3D11_USAGE_DYNAMIC:
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			auto result = deviceContext->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if(FAILED(result))
			{
				BE_ASSERT(false);
				return;
			}
				
			memcpy(mappedResource.pData, data, dataLen);
			deviceContext->Unmap(m_buffer, 0);
		}
		break;

		case D3D11_USAGE_IMMUTABLE:
		default:
		{
			BE_ASSERT(false);
		}
	}
}

void* beRenderBuffer::Map(beRenderInterface* ri)
{
	auto deviceContext = ri->GetDeviceContext();

	switch (m_d3dUsage)
	{
		case D3D11_USAGE_STAGING:
		case D3D11_USAGE_DYNAMIC:
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			auto result = deviceContext->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if(FAILED(result))
			{
				BE_ASSERT(false);
				return nullptr;
			}
			return mappedResource.pData;
		}
		break;

		case D3D11_USAGE_DEFAULT:
		case D3D11_USAGE_IMMUTABLE:
		default:
		{
			BE_ASSERT(false);
		}
	}
	return nullptr;
}

void beRenderBuffer::Unmap(beRenderInterface* ri)
{
	auto deviceContext = ri->GetDeviceContext();
	deviceContext->Unmap(m_buffer, 0);
}

void beRenderBuffer::Release()
{
	BE_SAFE_RELEASE(m_buffer);
	m_bufferSize = 0;
	m_numElements = 0;
	m_elementSize = 0;
}

void beRenderBuffer::StealBuffer(beRenderBuffer* that)
{
	Release();
	*this = *that;

	that->m_buffer = nullptr;
	that->m_numElements = 0;
}
