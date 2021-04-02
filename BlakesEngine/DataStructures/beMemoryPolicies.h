#pragma once
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beAssert.h"

#include <type_traits>
#include "../Core/beNonCopiable.h"
import beMath;

// Policies do not need to handle constructing/destructing elements, vector base does that. Default vector is hybrid<16>
namespace beMemoryPolicies
{
	template <typename Policy>
	struct Derived : Policy
	{
		using typename Policy::value_type;
		using Policy::Policy;
		using Policy::GetBuffer;
		using Policy::m_count;
	};

	template<typename T>
	concept MemoryPolicyDerivedImpl =
		std::is_default_constructible_v<Derived<T>> &&
		requires(Derived<T> policy)
	{
		{ policy.GetBuffer() }->std::same_as<typename T::value_type*>;
		{ policy.m_count }->std::convertible_to<int>;
	};
}


template<typename T>
concept MemoryPolicy = beMemoryPolicies::MemoryPolicyDerivedImpl<T>;

template<typename T>
concept VectorMemoryPolicy =
	MemoryPolicy<T> &&
	std::is_constructible_v<T, int, int, std::initializer_list<typename T::value_type>>;


#pragma warning(push)
#pragma warning(disable:26485) // Pointer arithmetic

template <typename T, int CAPACITY>
struct beVectorFixedPolicy
{
	public:
	static constexpr int DataSize() { return sizeof(m_buffer); }

	using value_type = T;
	beVectorFixedPolicy() = default;
	beVectorFixedPolicy(int capacity, int increaseBy, std::initializer_list<T> list)
	{
		BE_ASSERT(increaseBy == 0);
		BE_ASSERT(capacity <= CAPACITY); 
		
		int numToCopy = (int)list.size();
		BE_ASSERT(numToCopy <= capacity);

		std::uninitialized_copy(m_buffer, m_buffer + numToCopy, list.begin());
		m_count = numToCopy;
	}
	beVectorFixedPolicy(int capacity, int increaseBy)
	{
		BE_ASSERT(increaseBy == 0);
		BE_ASSERT(capacity <= CAPACITY);
	}

	void PolicyReserve(int capacity)
	{
		BE_ASSERT(capacity <= CAPACITY);
	}
	
	void PolicyRelease()
	{
		m_count = 0;
	}

	bool PolicyCheckRoomForAlloc() const
	{
		return m_count < CAPACITY;
	}

	constexpr int PolicyCapacity() const { return CAPACITY; }

	int m_count = 0;
	typename std::aligned_storage_t<sizeof(T), alignof(T)> m_buffer[CAPACITY];

	T* GetBuffer()
	{
		return (T*)m_buffer;
	}

	const T* GetBuffer() const
	{
		return (const T*)m_buffer;
	}
};

template <typename T>
struct beVectorFixedPolicy<T, 0>
{
	static constexpr int CAPACITY = 0;
	using value_type = T;
	static constexpr int DataSize() { return 0; }

	beVectorFixedPolicy() = default;
	beVectorFixedPolicy(int capacity, int increaseBy, std::initializer_list<T> list)
	{
		BE_ASSERT(increaseBy == 0);
		BE_ASSERT(capacity <= CAPACITY);
	}
	beVectorFixedPolicy(int capacity, int increaseBy)
	{
		BE_ASSERT(increaseBy == 0);
		BE_ASSERT(capacity <= CAPACITY);
	}
	protected:

	void PolicyReserve(int capacity)
	{
		BE_ASSERT(capacity <= CAPACITY);
	}

	void PolicyRelease() { }
	bool PolicyCheckRoomForAlloc() const { return false;  }
	constexpr int PolicyCapacity() const { return CAPACITY; }

	T* GetBuffer() { return nullptr; }
	const T* GetBuffer() const { return nullptr; }
	int m_count = 0;
};
static_assert(VectorMemoryPolicy<beVectorFixedPolicy<int, 1>>);
static_assert(VectorMemoryPolicy<beVectorFixedPolicy<int, 0>>);

template <typename T, int INITIAL_SIZE=8> // INTIAL_SIZE for consistency, also using for default constructor size
struct beVectorMallocPolicy : public NonCopiable
{
	using value_type = T;
	beVectorMallocPolicy() : beVectorMallocPolicy(INITIAL_SIZE, -1) {}
	beVectorMallocPolicy(int capacity, int increaseBy)
		: m_increaseBy(increaseBy)
	{
		PolicyReserve(capacity);
	}

	beVectorMallocPolicy(int capacity, int increaseBy, std::initializer_list<T> list)
	{
		int numToCopy = (int)list.size();
		BE_ASSERT(numToCopy <= capacity);

		PolicyReserve(capacity);
		std::uninitialized_copy(m_buffer, m_buffer + numToCopy, list.begin());
		m_count = numToCopy;
	}
	protected:


	void PolicyReserve(int capacity)
	{
		if (capacity > m_capacity)
		{
			T* newBuffer = (T*)BE_MALLOC_ALIGNED(alignof(T), sizeof(T)*capacity);
			if (m_count > 0)
			{
				BE_MEMCPY(newBuffer, m_buffer, sizeof(T)*m_count);
				BE_FREE_ALIGNED(m_buffer);
			}
			m_buffer = newBuffer;
			m_capacity = capacity;
		}
	}

	void PolicyRelease()
	{
		BE_FREE_ALIGNED(m_buffer);
		m_buffer = nullptr;
		m_count = 0;
		m_capacity = 0;
	}

	bool PolicyCheckRoomForAlloc()
	{
		if (m_count >= m_capacity)
		{
			switch (m_increaseBy)
			{
				case 0:
				{
					BE_ASSERT(false);
					return false;
				}
				case -1:
				{
					if (m_capacity == 0)
					{
						PolicyReserve(1);
						return true;
					}

					PolicyReserve(m_capacity * 2);
					return true;
				}
				default:
				{
					PolicyReserve(m_capacity + m_increaseBy);
					return true;
				}
			}
		}
		return true;
	}

	int PolicyCapacity() const { return m_capacity; }

	T* GetBuffer()
	{
		return m_buffer;
	}

	const T* GetBuffer() const
	{
		return m_buffer;
	}

	int m_count = 0;

	private:
	T* m_buffer = nullptr;
	int m_increaseBy = -1;
	int m_capacity = 0;
};
static_assert(VectorMemoryPolicy<beVectorMallocPolicy<int, 1>>);
static_assert(VectorMemoryPolicy<beVectorMallocPolicy<int, 0>>);

template <typename T, int RESERVED_SIZE = 16>
struct beVectorHybridPolicy : public NonCopiable
{
	using value_type = T;
	beVectorHybridPolicy() : beVectorHybridPolicy(0, -1) {}
	beVectorHybridPolicy(int capacity, int increaseBy, std::initializer_list<T> list)
	{
		int numToCopy = (int)list.size();
		BE_ASSERT(numToCopy <= capacity);

		PolicyReserve(capacity);
		std::uninitialized_copy(m_buffer, m_buffer + numToCopy, list.begin());
		m_count = numToCopy;
	}

	beVectorHybridPolicy(int capacity, int increaseBy)
		: m_increaseBy(increaseBy)
	{
		PolicyReserve(capacity);
	}
	protected:

	void PolicyReserve(int capacity)
	{
		if (capacity > m_capacity)
		{
			if (capacity <= RESERVED_SIZE)
			{
				m_buffer = (T*)m_storage;
			}
			else
			{
				T* newBuffer = (T*)BE_MALLOC_ALIGNED(alignof(T), sizeof(T)*capacity);
				if (m_count > 0)
				{
					BE_MEMCPY(newBuffer, m_buffer, sizeof(T)*m_count);
					if (m_buffer != (T*)m_storage)
					{
						BE_FREE_ALIGNED(m_buffer);
					}
				}
				m_buffer = newBuffer;
			}
			m_capacity = capacity;
		}
	}
	void PolicyRelease()
	{
		if (m_buffer != (T*)m_storage)
		{
			BE_FREE_ALIGNED(m_buffer);
			m_buffer = (T*)m_storage;
			m_capacity = RESERVED_SIZE;
		}
		m_count = 0;
	}

	bool PolicyCheckRoomForAlloc()
	{
		if (m_count >= m_capacity)
		{
			switch (m_increaseBy)
			{
				case 0:
				{
					BE_ASSERT(false);
					return false;
				}
				case -1:
				{
					if (m_capacity == 0)
					{
						PolicyReserve(1);
						return true;
					}

					PolicyReserve(m_capacity * 2);
					return true;
				}
				default:
				{
					PolicyReserve(m_capacity + m_increaseBy);
					return true;
				}
			}
		}
		return true;
	}

	int PolicyCapacity() const { return m_capacity; }

	T* GetBuffer()
	{
		return m_buffer;
	}

	const T* GetBuffer() const
	{
		return m_buffer;
	}

	typename std::aligned_storage_t<sizeof(T), alignof(T)> m_storage[RESERVED_SIZE];

	int m_count = 0;

	private:
	T* m_buffer = nullptr;
	int m_increaseBy = -1;
	int m_capacity = 0;
};

template <typename T>
struct beVectorHybridPolicy<T, 0> : public beVectorMallocPolicy<T, 0>
{
	using Base = beVectorMallocPolicy<T, 0>;

	using Base::Base;
	using typename Base::value_type;
	using Base::GetBuffer;
	using Base::m_count;
};

static_assert(VectorMemoryPolicy<beVectorHybridPolicy<int, 1>>);
static_assert(VectorMemoryPolicy<beVectorHybridPolicy<int, 0>>);


template <typename T, int INITIAL_SIZE=8> // INTIAL_SIZE for consistency, also using for default constructor size
struct beAssignableMallocPolicy
{
	using value_type = T;
	beAssignableMallocPolicy() : beAssignableMallocPolicy(INITIAL_SIZE) {}
	beAssignableMallocPolicy(int capacity)
	{
		PolicyReserve(capacity);
	}
	beAssignableMallocPolicy(const beAssignableMallocPolicy& rhs) = delete;
	beAssignableMallocPolicy(beAssignableMallocPolicy&& rhs)
	{
		m_buffer = rhs.m_buffer;
		m_count = rhs.m_count;
		m_capacity = rhs.m_capacity;
		m_ownsBuffer = rhs.m_ownsBuffer;

		rhs.m_buffer = nullptr;
		rhs.m_count = 0;
		rhs.m_capacity = 0;
		rhs.m_ownsBuffer = false;
	}

	beAssignableMallocPolicy(void* buffer, int bufferLen, bool takeOwnership)
	{
		m_buffer = (T*)buffer;
		m_count = 0;
		m_capacity = bufferLen;
		m_ownsBuffer = takeOwnership;
	}

	beAssignableMallocPolicy(const void* buffer, int bufferLen)
		: beAssignableMallocPolicy(const_cast<void*>(buffer), bufferLen, false)
	{
	}
	protected:

	void PolicyReserve(int capacity, bool ensureOwnBuffer=false)
	{
		if (capacity > m_capacity || (ensureOwnBuffer && !m_ownsBuffer))
		{
			const int allocSize = Max(capacity, m_count);
			T* newBuffer = (T*)BE_MALLOC_ALIGNED(alignof(T), sizeof(T)*allocSize);
			if (m_count > 0)
			{
				BE_MEMCPY(newBuffer, m_buffer, sizeof(T)*m_count);
			}
			BE_FREE_ALIGNED(m_buffer);
			m_buffer = newBuffer;
			m_capacity = allocSize;
			m_ownsBuffer = true;
		}
	}

	void ReserveNoCopy(int capacity, bool ensureOwnBuffer=false)
	{
		if (capacity > m_capacity || (ensureOwnBuffer && !m_ownsBuffer))
		{
			int allocSize = Max(capacity, m_count);
			T* newBuffer = (T*)BE_MALLOC_ALIGNED(alignof(T), sizeof(T)*allocSize);
			BE_FREE_ALIGNED(m_buffer);
			m_buffer = newBuffer;
			m_capacity = allocSize;
			m_ownsBuffer = true;
		}
	}

	void AssignBuffer(const void* buffer, int bufferLen)
	{
		AssignBuffer(const_cast<void*>(buffer), bufferLen, false);
	}

	void AssignBuffer(void* buffer, int bufferLen, bool takeOwnership)
	{
		PolicyRelease();
		m_buffer = (T*)buffer;
		m_capacity = bufferLen;
		m_count = bufferLen;
	}

	void PolicyRelease()
	{
		if (m_ownsBuffer)
		{
			BE_FREE_ALIGNED(m_buffer);
		}
		m_buffer = nullptr;
		m_count = 0;
		m_capacity = 0;
		m_ownsBuffer = false;
	}

	bool PolicyCheckRoomForAlloc()
	{
		return m_count < m_capacity;
	}

	int PolicyCapacity() const { return m_capacity; }

	T* GetBuffer()
	{
		return m_buffer;
	}

	const T* GetBuffer() const
	{
		return m_buffer;
	}

	int m_count = 0;

	private:
	T* m_buffer = nullptr;
	int m_capacity = 0;
	bool m_ownsBuffer = false;
};
static_assert(MemoryPolicy<beAssignableMallocPolicy<int, 1>>);

#pragma warning(pop)
