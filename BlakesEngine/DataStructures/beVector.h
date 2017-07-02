#pragma once
#include "blakesengine/core/beMacros.h"
#include "blakesengine/core/beAssert.h"

#include <type_traits>

// Policies do not need to handle constructing/destructing elements, vector base does that. Default vector is hybrid<16>


template <typename T, int CAPACITY>
struct beVectorFixedPolicy
{
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

	void Reserve(int capacity)
	{
		BE_ASSERT(capacity <= CAPACITY);
	}
	
	void Release()
	{
	}

	bool CheckRoomForAlloc() const
	{
		return m_count < CAPACITY;
	}

	int Capacity() const { return CAPACITY; }

	int m_count = 0;
	typename std::aligned_storage_t<sizeof(T), alignof(T)> m_buffer[CAPACITY];
};


template <typename T, int INITIAL_SIZE=8> // INTIAL_SIZE for consistency, also using for default constructor size
struct beVectorMallocPolicy : public NonCopiable
{
	protected:
	beVectorMallocPolicy() : beVectorMallocPolicy(INITIAL_SIZE, -1) {}
	beVectorMallocPolicy(int capacity, int increaseBy, std::initializer_list<T> list)
	{
		BE_ASSERT((int)list.size() <= capacity);
		Reserve(capacity);
		for (const T& iter : list)
		{
			BE_NEW(m_buffer+m_count++) T(iter);
		}
	}

	beVectorMallocPolicy(int capacity, int increaseBy)
		: m_increaseBy(increaseBy)
	{
		Reserve(capacity);
	}


	void Reserve(int capacity)
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

	void Release()
	{
		BE_FREE_ALIGNED(m_buffer);
	}

	bool CheckRoomForAlloc()
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
						Reserve(1);
						return true;
					}

					Reserve(m_capacity * 2);
					return true;
				}
				default:
				{
					Reserve(m_capacity + m_increaseBy);
					return true;
				}
			}
		}
		return true;
	}

	int Capacity() const { return m_capacity; }

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
		Reserve(capacity);
		for (const T& iter : list)
		{
			BE_NEW(m_buffer+m_count++) T(iter);
		}
	}

	beVectorHybridPolicy(int capacity, int increaseBy)
		: m_increaseBy(increaseBy)
	{
		Reserve(capacity);
	}

	void Reserve(int capacity)
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
	void Release()
	{
		if (m_buffer != (T*)m_storage)
		{
			BE_FREE_ALIGNED(m_buffer);
		}
	}

	bool CheckRoomForAlloc()
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
						Reserve(1);
						return true;
					}

					Reserve(m_capacity * 2);
					return true;
				}
				default:
				{
					Reserve(m_capacity + m_increaseBy);
					return true;
				}
			}
		}
		return true;
	}

	int Capacity() const { return m_capacity; }

	typename std::aligned_storage_t<sizeof(T), alignof(T)> m_storage[RESERVED_SIZE];

	T* m_buffer = nullptr;
	int m_count = 0;
	int m_increaseBy = -1;
	int m_capacity = 0;
};


// If increaseBy == -1, double size, if increaseBy == 0, do not increase
template<typename T, typename Policy>
class beVectorBase : protected Policy
{

	public:
		typedef T value_type;
		enum { element_size = sizeof(T) };
		
		beVectorBase() = default;
		beVectorBase(int capacity, int increaseBy, std::initializer_list<T> list) : Policy(capacity, increaseBy, list) {}
		beVectorBase(int capacity, int increaseBy)
			: Policy(capacity, increaseBy)
		{
		}
		
		~beVectorBase()
		{
			Release();
		}

		template <typename... Args>
		void SetAllTo(Args&&... args)
		{
			for (int i = 0; i < m_count; i++)
			{

			}
		}
		
		void Release()
		{
			DestructElements(0, m_count-1);
			m_count = 0;
			Policy::Release();
		}
		
		void ReleaseUninitialised()
		{
			m_count = 0;
			Policy::Release();
		}

		void Reserve(int capacity)
		{
			Policy::Reserve(capacity);
		}
		
		void ReserveAndSetCount(int count)
		{
			Reserve(count);
			SetCount(count);
		}

		template <typename... Args>
		void ReserveAndSetCount(int count, Args... args)
		{
			Reserve(count);
			SetCount(count, std::forward<Args>(args)...);
		}

		void ReserveAndSetCountUninitialised(int count)
		{
			Reserve(count);
			SetCountUninitialised(count);
		}

		void SetCount(int count)
		{
			const int currentCount = m_count;
			BE_ASSERT(count <= Capacity());
			if (count > currentCount) // growing
			{
				ConstructElements(currentCount, count);
			}
			else if (count < currentCount) // shrinking
			{
				DestructElements(count, currentCount-1);
			}
			m_count = count;
		}
		
		template <typename... Args>
		void SetCount(int count, Args&&... defaultVal)
		{
			const int currentCount = m_count;
			BE_ASSERT(count <= Capacity());
			if (count > currentCount) // growing
			{
				for (int i = currentCount; i < count; i++)
				{
					new(&m_buffer[i]) value_type(std::forward<Args>(defaultVal)...);
				}
			}
			else if (count < currentCount) // shrinking
			{
				DestructElements(count, currentCount-1);
			}
			m_count = count;
		}

		void SetCountUninitialised(int count)
		{
			BE_ASSERT(count <= Capacity());
			m_count = count;
		}

		int Count() const
		{
			return Policy::m_count;
		}
		
		int Capacity() const
		{
			return Policy::Capacity();
		}
		
		int Insert(T& that)
		{
			if (!CheckRoomForAlloc())
			{
				return -1;
			}

			int index = m_count++;
			BE_NEW(&m_buffer[index]) T(that);
			return index;
		}

		bool Contains(T* ptr) const
		{
			return ptr >= begin() && ptr < end();
		}

		void Remove(T* ptr)
		{
			BE_ASSERT(Contains(ptr));
			Remove(ptr - begin());
		}

		void QuickRemove(T* ptr)
		{
			BE_ASSERT(Contains(ptr));
			QuickRemove((int)(ptr - begin()));
		}
		
		void Remove(int index)
		{
			BE_ASSERT(index >= 0 && index < m_count);
			DestructElements(index, index);
			const int endIndex = m_count - 1;
			const int numToMove = endIndex - index;
			if (numToMove > 0)
			{
				memmove(&m_buffer[index], &m_buffer[index+1], sizeof(T) * numToMove);
			}
			m_count--;
		}

		void QuickRemove(int index)
		{
			BE_ASSERT(index >= 0 && index < m_count);
			DestructElements(index, index);
			const int endIndex = m_count - 1;
			if (index < endIndex)
			{
				memcpy(&m_buffer[index], &m_buffer[endIndex], sizeof(T));
			}
			m_count--;
		}

		T* AllocateNew()
		{
			if (!CheckRoomForAlloc())
			{
				return nullptr;
			}
			int index = m_count++;
			return (T*)&m_buffer[index];
		}

		template<class ...Args>
		T* AddNew(Args&&... args)
		{
			T* obj = AllocateNew();
			return BE_NEW(obj) T{std::forward<Args>(args)...};
		}

		T& operator[](int i)
		{
			return At(i);
		}

		const T& operator[](int i) const
		{
			return At(i);
		}

		T& At(int i)
		{
			BE_ASSERT(i <= Count());
			return *(T*)&begin()[i];
		}

		const T& At(int i) const
		{
			BE_ASSERT(i <= Count());
			return *(T*)&begin()[i];
		}

		T* begin()
		{
			return (T*)Policy::m_buffer;
		}

		T* end()
		{
			return ((T*)Policy::m_buffer) + m_count;
		}

		const T* begin() const
		{
			return (const T*)Policy::m_buffer;
		}

		const T* end() const
		{
			return ((const T*)Policy::m_buffer) + m_count;
		}

		typedef bool (*CompareFn)(const T*, const T*, int*);
		template <CompareFn fn> //CompareFn return true if match, else arg3 will be <0 if lhs<rhs or >0 if lhs<rhs
		bool BinarySearch(const T* target, const T** result) const
		{
			*result = nullptr;
			if (m_count == 0)
			{
				return false;
			}

			int lowerBound = 0;
			int upperBound = m_count-1;
			int index = upperBound / 2;
			while (true)
			{
				const T* entry = &At(index);
				
				int diff;
				bool res = fn(target, entry, &diff);
				if (res)
				{
					*result = entry;
					return true;
				}
				if (diff > 0)
				{
					lowerBound = index;
				}
				else
				{
					upperBound = index;
				}
		
				if (upperBound == lowerBound)
				{
					return false;
				}
				int newIndex = (lowerBound + upperBound) / 2;
				if (index == newIndex)
				{
					newIndex++;
				}
				index = newIndex;
			}
		}

		bool CheckRoomForAlloc()
		{
			return Policy::CheckRoomForAlloc();
		}

	private:
		void ConstructElements(int startElement, int endElement)
		{
			if (!std::is_trivially_constructible_v<T>)
			{
				for (int i = startElement; i < endElement; i++)
				{
					BE_NEW(m_buffer + i) T();
				}
			}
		}
		
		void DestructElements(int startElement, int endElement)
		{
			if (!std::is_trivially_destructible_v<T>)
			{
				for (int i = startElement; i <= endElement; i++)
				{
						At(i).~T();
				}
			}
		}
};

template <typename T, int HYBRID_CAPACITY=16>
class beVector : public beVectorBase<T, beVectorHybridPolicy<T, HYBRID_CAPACITY>>
{
	typedef beVectorBase<T, beVectorHybridPolicy<T, HYBRID_CAPACITY>> Base;
	public:
	explicit beVector(int capacity=HYBRID_CAPACITY, int increaseBy=-1) : Base(capacity, increaseBy) {}
	explicit beVector(int capacity, int count, int increaseBy) : Base(capacity, increaseBy) { Base::SetCount(count); }
	beVector(int capacity, int increaseBy, std::initializer_list<T> list) : Base(capacity, increaseBy, list) {}
};

template <typename T>
class beHeapVector : public beVectorBase<T, beVectorMallocPolicy<T>>
{
	typedef beVectorBase<T, beVectorMallocPolicy<T>> Base;
	public:
	explicit beHeapVector(int capacity, int increaseBy=-1) : Base(capacity, increaseBy) {}
	explicit beHeapVector(int capacity, int count, int increaseBy) : Base(capacity, increaseBy) { Base::SetCount(count); }
	beHeapVector(int capacity, int increaseBy, std::initializer_list<T> list) : Base(capacity, increaseBy, list) {}
};

template <typename T, int CAPACITY>
class beFixedVector : public beVectorBase<T, beVectorFixedPolicy<T, CAPACITY>>
{
	typedef beVectorBase<T, beVectorFixedPolicy<T, CAPACITY>> Base;
	public:
	beFixedVector() = default;
	beFixedVector(std::initializer_list<T> list) : Base(CAPACITY, 0, list) {}
};
