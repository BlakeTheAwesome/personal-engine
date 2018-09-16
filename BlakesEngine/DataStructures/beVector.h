#pragma once
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beAssert.h"
#include "beMemoryPolicies.h"

#include <type_traits>

#pragma warning(push)
#pragma warning(disable:26481) // pointer arithmetic
#pragma warning(disable:26482) // pointer arithmetic
#pragma warning(disable:26485) // pointer arithmetic
#pragma warning(disable:26446) // pointer arithmetic

// If increaseBy == -1, double size, if increaseBy == 0, do not increase
template<typename T, typename Policy>
class beVectorBase : protected Policy
{

	public:
		using value_type = T;
		enum { element_size = sizeof(T) };
		
		beVectorBase() = default;
		beVectorBase(int capacity, int increaseBy, std::initializer_list<T> list) : Policy(capacity, increaseBy, list) {}
		beVectorBase(int capacity, int increaseBy)
			: Policy(capacity, increaseBy)
		{
		}
		beVectorBase(const beVectorBase&) = default;
		beVectorBase(beVectorBase&&) = default;
		beVectorBase& operator=(const beVectorBase&) = default;
		beVectorBase& operator=(beVectorBase&&) = default;

		~beVectorBase()
		{
			Release();
		}

		void Release()
		{
			DestructElements(0, m_count-1);
			PolicyRelease();
		}

		void Clear()
		{
			DestructElements(0, m_count-1);
			m_count = 0;
		}
		
		void ReleaseUninitialised()
		{
			PolicyRelease();
		}

		void Reserve(int capacity)
		{
			PolicyReserve(capacity);
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
			return PolicyCapacity();
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

		void AddRange(const T* range, int rangeLength)
		{
			ReserveAndSetCountUninitialised(m_count + rangeLength);
			T* ptr = end() - rangeLength;
			for (int i : RangeIter(rangeLength))
			{
				BE_NEW(ptr+i) T(range[i]);
			}
		}

		template <typename U>//, typename std::enable_if_t<!std::is_convertible_v<decltype(std::declval<T>() == std::declval<U>()), bool>> = 0>
		T* AddressOf(U&& comp)
		{
			auto ret = std::find_if(begin(), end(), std::forward<U>(comp));
			return ret != end() ? ret : nullptr;
		}

		template <typename U>//, typename std::enable_if_t<!std::is_convertible_v<decltype(std::declval<T>() == std::declval<U>()), bool>> = 0>
		const T* AddressOf(U&& comp) const
		{
			auto ret = std::find_if(begin(), end(), std::forward<U>(comp));
			return ret != end() ? ret : nullptr;
		}

		template <typename U>
		T* AddressOf(const U& val)
		{
			auto ret = std::find(begin(), end(), val);
			return ret != end() ? ret : nullptr;
		}

		template <typename U>
		const T* AddressOf(const U& val) const
		{
			auto ret = std::find(begin(), end(), val);
			return ret != end() ? ret : nullptr;
		}

		template <typename U>
		int IndexOf(U&& comp) const
		{
			auto address = AddressOf(std::forward<U>(comp));
			return address ? (int)std::distance(begin(), address) : -1;
		}

		template <int ARRAY_LEN>
		void AddRange(const T (&range)[ARRAY_LEN])
		{
			AddRange(range, ARRAY_LEN);
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

		T& First()
		{
			BE_ASSERT(Count() > 0);
			return *begin();
		}

		const T& First() const
		{
			BE_ASSERT(Count() > 0);
			return *begin();
		}

		T& Last()
		{
			BE_ASSERT(Count() > 0);
			return *(end() - 1);
		}

		const T& Last() const
		{
			BE_ASSERT(Count() > 0);
			return *(end() - 1);
		}

		using CompareFn = bool (*)(const T*, const T*, int*);
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
				
				int diff = 0;
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
			return PolicyCheckRoomForAlloc();
		}

	private:
		using Policy::m_buffer;
		using Policy::m_count;

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
	using Base = beVectorBase<T, beVectorHybridPolicy<T, HYBRID_CAPACITY>>;
	public:
	explicit beVector(int capacity=HYBRID_CAPACITY, int increaseBy=-1) : Base(capacity, increaseBy) {}
	explicit beVector(int capacity, int count, int increaseBy) : Base(capacity, increaseBy) { Base::SetCount(count); }
	beVector(int capacity, int increaseBy, std::initializer_list<T> list) : Base(capacity, increaseBy, list) {}

	~beVector() = default;
	beVector(const beVector&) = delete;
	beVector(beVector&&) = delete;
	beVector& operator=(const beVector&) = delete;
	beVector& operator=(beVector&&) = delete;
};

template <typename T, int INITIAL_SIZE=8>
class beHeapVector : public beVectorBase<T, beVectorMallocPolicy<T, INITIAL_SIZE>>
{
	using Base = beVectorBase<T, beVectorMallocPolicy<T>>;
	public:
	beHeapVector() = default;
	explicit beHeapVector(int capacity, int increaseBy=-1) : Base(capacity, increaseBy) {}
	explicit beHeapVector(int capacity, int count, int increaseBy) : Base(capacity, increaseBy) { Base::SetCount(count); }
	beHeapVector(int capacity, int increaseBy, std::initializer_list<T> list) : Base(capacity, increaseBy, list) {}

	~beHeapVector() = default;
	beHeapVector(const beHeapVector&) = delete;
	beHeapVector(beHeapVector&&) = delete;
	beHeapVector& operator=(const beHeapVector&) = delete;
	beHeapVector& operator=(beHeapVector&&) = delete;
};

template <typename T, int CAPACITY>
class beFixedVector : public beVectorBase<T, beVectorFixedPolicy<T, CAPACITY>>
{
	using Base = beVectorBase<T, beVectorFixedPolicy<T, CAPACITY>>;
	public:
	using Base::DataSize;

	beFixedVector() = default;
	beFixedVector(std::initializer_list<T> list) : Base(CAPACITY, 0, list) {}

	~beFixedVector() = default;
	beFixedVector(const beFixedVector&) = delete;
	beFixedVector(beFixedVector&&) = delete;
	beFixedVector& operator=(const beFixedVector&) = delete;
	beFixedVector& operator=(beFixedVector&&) = delete;
};
#pragma warning(pop)
