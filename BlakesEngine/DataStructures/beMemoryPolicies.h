#pragma once
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beAssert.h"

#include <type_traits>
#include "BlakesEngine/Math/beMath.h"

// Policies do not need to handle constructing/destructing elements, vector base does that. Default vector is hybrid<16>

#pragma warning(push)
#pragma warning(disable:26485) // Pointer arithmetic

template <typename T, int CAPACITY>
struct beVectorFixedPolicy
{
	public:
	static constexpr int DataSize() { return sizeof(m_buffer); }

	protected:
	beVectorFixedPolicy() = default;
	beVectorFixedPolicy(int capacity, int increaseBy, std::initializer_list<T> list)
	{
		BE_ASSERT(increaseBy == 0);
		BE_ASSERT(capacity <= CAPACITY); 
		BE_ASSERT((int)list.size() <= capacity);

		for (const T& iter : list)
		{
			BE_NEW(m_buffer+m_count++) T(iter);
		}
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
};


template <typename T, int INITIAL_SIZE=8> // INTIAL_SIZE for consistency, also using for default constructor size
struct beVectorMallocPolicy : public NonCopiable
{
	protected:
	beVectorMallocPolicy() : beVectorMallocPolicy(INITIAL_SIZE, -1) {}
	beVectorMallocPolicy(int capacity, int increaseBy)
		: m_increaseBy(increaseBy)
	{
		PolicyReserve(capacity);
	}

	beVectorMallocPolicy(int capacity, int increaseBy, std::initializer_list<T> list)
	{
		BE_ASSERT((int)list.size() <= capacity);
		PolicyReserve(capacity);
		for (const T& iter : list)
		{
			BE_NEW(m_buffer+m_count++) T(iter);
		}
	}


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

	T* m_buffer = nullptr;
	int m_count = 0;
	int m_increaseBy = -1;
	int m_capacity = 0;
};


template <typename T, int RESERVED_SIZE = 16>
struct beVectorHybridPolicy : public NonCopiable
{
	protected:
	beVectorHybridPolicy() : beVectorHybridPolicy(0, -1) {}
	beVectorHybridPolicy(int capacity, int increaseBy, std::initializer_list<T> list)
	{
		BE_ASSERT((int)list.size() <= capacity);
		PolicyReserve(capacity);
		for (const T& iter : list)
		{
			BE_NEW(m_buffer+m_count++) T(iter);
		}
	}

	beVectorHybridPolicy(int capacity, int increaseBy)
		: m_increaseBy(increaseBy)
	{
		PolicyReserve(capacity);
	}

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

	typename std::aligned_storage_t<sizeof(T), alignof(T)> m_storage[RESERVED_SIZE];

	T* m_buffer = nullptr;
	int m_count = 0;
	int m_increaseBy = -1;
	int m_capacity = 0;
};


template <typename T, int INITIAL_SIZE=8> // INTIAL_SIZE for consistency, also using for default constructor size
struct beAssignableMallocPolicy
{
	protected:
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

	void PolicyReserve(int capacity, bool ensureOwnBuffer=false)
	{
		if (capacity > m_capacity || (ensureOwnBuffer && !m_ownsBuffer))
		{
			const int allocSize = beMath::Max(capacity, m_count);
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
			int allocSize = beMath::Max(capacity, m_count);
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

	T* m_buffer = nullptr;
	int m_count = 0;
	int m_capacity = 0;
	bool m_ownsBuffer = false;
};
#pragma warning(pop)
