#pragma once
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beAssert.h"
#include "beMemoryPolicies.h"

#include <type_traits>
#include <span>

// If increaseBy == -1, double size, if increaseBy == 0, do not increase
template<MemoryPolicy Policy>
class beDataBufferBase : protected Policy
{
	public:
	using value_type = u8;
	using Policy::m_count;

	enum { element_size = sizeof(value_type) };

	beDataBufferBase() = default;
	beDataBufferBase(const beDataBufferBase&) = delete;
	beDataBufferBase(beDataBufferBase&&) = default;

	explicit beDataBufferBase(int capacity) : Policy(capacity) { m_count = capacity; }
	beDataBufferBase(void* buffer, int bufferLen, bool takeOwnership) : Policy(buffer, bufferLen, takeOwnership) { m_count = bufferLen; }
	beDataBufferBase(const void* buffer, int bufferLen) : Policy(buffer, bufferLen) { m_count = bufferLen; }

	std::span<const u8> ToSpan() const { return {Policy::GetBuffer(), m_count}; }
	std::span<u8> ModifySpan() { return {Policy::GetBuffer(), m_count}; }

	int GetSize() const { return m_count; }
	const u8* GetBuffer() const { return Policy::GetBuffer(); }
	u8* ModifyBuffer() { return Policy::GetBuffer(); }

	void ResizeBuffer(int size)
	{
		Policy::PolicyReserve(size);
		m_count = size;
	}
};

class beDataBuffer : public beDataBufferBase<beAssignableMallocPolicy<u8, 0>>
{
	using Base = beDataBufferBase<beAssignableMallocPolicy<u8, 0>>;
	public:
	beDataBuffer() = default;
	explicit beDataBuffer(int capacity) : Base(capacity) {}
	beDataBuffer(void* buffer, int bufferLen, bool takeOwnership) : Base(buffer, bufferLen, takeOwnership) {}
	beDataBuffer(const void* buffer, int bufferLen) : Base(buffer, bufferLen) {}
};
