#pragma once
#include "BlakesEngine/Core/beNonCopiable.h"
#include "beRendering.h"

struct ID3D11Buffer;
class beRenderInterface;

class beRenderBuffer : public NonCopiable
{
	public:

	beRenderBuffer() = default;
	~beRenderBuffer() { Release(); }

	beRenderBuffer(const beRenderBuffer&) = delete;
	beRenderBuffer(beRenderBuffer&&) = delete;
	beRenderBuffer& operator=(beRenderBuffer&& rhs) noexcept { StealBuffer(&rhs); return *this; }

	bool Allocate(beRenderInterface* ri, int elementSize, int numElements, int d3dUsage, u32 d3dBindFlags, beRendering::Topology d3dIndexTopology, u32 d3dCPUAccessFlags, u32 d3dMiscFlags, void* initialData=nullptr);
	bool Allocate(beRenderInterface* ri, int elementSize, int numElements, int d3dUsage, u32 d3dBindFlags, int d3dIndexTopology, u32 d3dCPUAccessFlags, u32 d3dMiscFlags, void* initialData=nullptr);
	void Release();

	void Update(beRenderInterface* ri, void* data, int dataLen);
	void* Map(beRenderInterface* ri);
	void Unmap(beRenderInterface* ri);

	void StealBuffer(beRenderBuffer* that);
	
	bool IsValid() const { return m_buffer != nullptr; }
	ID3D11Buffer* GetBuffer() const { return m_buffer; }

	int BufferSize() const { return m_bufferSize; }
	int NumElements() const { return m_numElements; }
	int ElementSize() const { return m_elementSize; }
	int D3DUsage() const { return m_d3dUsage; }
	int D3DBindFlags() const { return m_d3dBindFlags; }
	int D3DCPUAccessFlags() const { return m_d3dCPUAccessFlags; }
	int D3DMiscFlags() const { return m_d3dMiscFlags; }
	int D3DIndexTopology() const { return m_d3dIndexTopology; }

	private:
	beRenderBuffer& operator=(const beRenderBuffer& rhs) = default;
	ID3D11Buffer* m_buffer = nullptr;
	int m_bufferSize = 0;
	int m_numElements = 0;
	int m_elementSize = 0;
	int m_d3dUsage = 0;
	int m_d3dIndexTopology = 0;
	u32 m_d3dBindFlags = 0;
	u32 m_d3dCPUAccessFlags = 0;
	u32 m_d3dMiscFlags = 0;
};
