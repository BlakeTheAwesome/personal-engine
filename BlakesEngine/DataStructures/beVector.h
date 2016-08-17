#pragma once
#include "blakesengine/core/beMacros.h"
#include "blakesengine/core/beAssert.h"

#include <type_traits>

template <typename T, int CAPACITY>
struct beVectorFixedPolicy
{
	beVectorFixedPolicy() = default;
	beVectorFixedPolicy(int capacity, int increaseBy, const std::initializer_list<T>& list)
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
		BE_ASSERT(capacity <= CAPACITY)
	}

	void Reserve(int capacity)
	{
		BE_ASSERT(capacity <= CAPACITY);
	}
	void Release(){}
	bool CheckRoomForAlloc() const
	{
		return m_count < CAPACITY;
	}

	int Capacity() const { return CAPACITY; }

	int m_count = 0;
	typename std::aligned_storage<sizeof(T), alignof(T)>::type m_buffer[CAPACITY];
};

template <typename T>
struct beVectorMallocPolicy : public NonCopiable
{
	beVectorMallocPolicy() = delete;
	beVectorMallocPolicy(int capacity, int increaseBy, const std::initializer_list<T>& list)
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

// If increaseBy == -1, double size, if increaseBy == 0, do not increase
template<typename T, typename Policy>
class beVectorBase : Policy
{
	public:
		typedef T value_type;
		enum { element_size = sizeof(T) };
		
		beVectorBase() = default;
		beVectorBase(int capacity, int increaseBy, const std::initializer_list<T>& list) : Policy(capacity, increaseBy, list) {}
		beVectorBase(int capacity, int increaseBy)
			: Policy(capacity, increaseBy)
		{
		}
		
		~beVectorBase()
		{
			Release();
		}
		
		void Release()
		{
			DestructElements(0, m_count-1);
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

		void SetCount(int count)
		{
			const int capacity = Capacity();
			BE_ASSERT(count <= capacity);
			if (count > capacity) // growing
			{
				ConstructElements(capacity, count);
			}
			else if (count < capacity) // shrinking
			{
				DestructElements(count, m_count-1);
			}
			m_count = count;
		}
		
		int Count() const
		{
			return m_count;
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
		T* AddNew(Args... args)
		{
			T* obj = AllocateNew();
			return BE_NEW(obj) T{args...};
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
			BE_ASSERT(i <= m_count);
			return *(T*)&m_buffer[i];
		}

		const T& At(int i) const
		{
			BE_ASSERT(i <= m_count);
			return *(T*)&m_buffer[i];
		}

		T* begin()
		{
			return (T*)m_buffer;
		}

		T* end()
		{
			return (T*)m_buffer + m_count;
		}

		const T* begin() const
		{
			return (const T*)m_buffer;
		}

		const T* end() const
		{
			return (const T*)m_buffer + m_count;
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
		
	private:
		template <bool isPod=std::is_pod<T>::value> void ConstructElements(int startElement, int endElement);
		template<> void ConstructElements<true>(int startElement, int endElement) {}
		template<> void ConstructElements<false>(int startElement, int endElement)
		{
			for (int i = startElement; i < endElement; i++)
			{
				BE_NEW(m_buffer + i) T();
			}
		}
		
		template <bool isPod=std::is_pod<T>::value> void DestructElements(int startElement, int endElement);
		template<> void DestructElements<true>(int startElement, int endElement) {}
		template<> void DestructElements<false>(int startElement, int endElement)
		{
			for (int i = startElement; i <= endElement; i++)
			{
					At(i).~T();
			}
		}

		bool CheckRoomForAlloc()
		{
			return Policy::CheckRoomForAlloc();
		}
};

template <typename T>
class beVector : public beVectorBase<T, beVectorMallocPolicy<T>>
{
	public:
	explicit beVector(int capacity, int increaseBy=-1) : beVectorBase(capacity, increaseBy) {}
	explicit beVector(int capacity, int count, int increaseBy) : beVectorBase(capacity, increaseBy) { SetCount(count); }
	beVector(int capacity, int increaseBy, const std::initializer_list<T>& list) : beVectorBase(capacity, increaseBy, list) {}
};

template <typename T, int CAPACITY>
class beFixedVector : public beVectorBase<T, beVectorFixedPolicy<T, CAPACITY>>
{
	public:
	beFixedVector() = default;
	beFixedVector(const std::initializer_list<T>& list) : beVectorBase(CAPACITY, 0, list) {}
};
