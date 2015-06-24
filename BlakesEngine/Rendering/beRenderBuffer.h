#pragma once
#include "BlakesEngine/Core/beNonCopiable.h"

struct ID3D11Buffer;
class beRenderInterface;

class beRenderBuffer : public NonCopiable
{
	public:
	beRenderBuffer();
	~beRenderBuffer();

	bool Allocate(beRenderInterface* ri, int elementSize, int numElements, int d3dUsage, u32 d3dBindFlags, u32 d3dCPUAccessFlags, u32 d3dMiscFlags, void* initialData=nullptr);
	void Release();

	void Update(beRenderInterface* ri, void* data, int dataLen);
	void* Map(beRenderInterface* ri);
	void Unmap(beRenderInterface* ri);

	void Set(beRenderBuffer& that);

	bool IsValid() const { return m_buffer != nullptr; }
	ID3D11Buffer* GetBuffer() const { return m_buffer; }

	int BufferSize() const { return m_bufferSize; }
	int NumElements() const { return m_numElements; }
	int ElementSize() const { return m_elementSize; }
	int D3DUsage() const { return m_d3dUsage; }
	int D3DBindFlags() const { return m_d3dBindFlags; }
	int D3DCPUAccessFlags() const { return m_d3dCPUAccessFlags; }
	int D3DMiscFlags() const { return m_d3dMiscFlags; }

	private:
	ID3D11Buffer* m_buffer;
	int m_bufferSize;
	int m_numElements;
	int m_elementSize;
	int m_d3dUsage;
	u32 m_d3dBindFlags;
	u32 m_d3dCPUAccessFlags;
	u32 m_d3dMiscFlags;
};
