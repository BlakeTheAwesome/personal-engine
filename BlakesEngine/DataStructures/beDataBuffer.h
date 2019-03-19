#pragma once
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beAssert.h"
#include "beMemoryPolicies.h"

#include <type_traits>

// If increaseBy == -1, double size, if increaseBy == 0, do not increase
template<typename Policy>
class beDataBufferBase : protected Policy
{
	public:
	using value_type = u8;
	enum { element_size = sizeof(value_type) };

	beDataBufferBase() = default;
	beDataBufferBase(const beDataBufferBase&) = delete;
	beDataBufferBase(beDataBufferBase&&) = default;

	explicit beDataBufferBase(int capacity) : Policy(capacity) {}
	beDataBufferBase(void* buffer, int bufferLen, bool takeOwnership) : Policy(buffer, bufferLen, takeOwnership) {}
	beDataBufferBase(const void* buffer, int bufferLen) : Policy(buffer, bufferLen) {}

	gsl::span<const u8> ToSpan() const { return {Policy::m_buffer, Policy::m_count}; }
	gsl::span<u8> ModifySpan() { return {Policy::m_buffer, Policy::m_count}; }

	int GetSize() const { return Policy::m_count; }
	const u8* GetBuffer() const { return Policy::m_buffer; }
	u8* ModifyBuffer() { return Policy::m_buffer; }

	void ResizeBuffer(int size) { Policy::PolicyReserve(size); }
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
