#pragma once
#include "BlakesEngine/Core/beConcepts.h"
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
	using Policy::GetBuffer;
	public:
		using value_type = T;
		using iterator = T*;
		using const_iterator = const T*;
		enum { element_size = sizeof(T) };
		
		beVectorBase() = default;
		beVectorBase(int capacity, int increaseBy, std::initializer_list<T> list) : Policy(capacity, increaseBy, list) {}
		beVectorBase(int capacity, int increaseBy)
			: Policy(capacity, increaseBy)
		{
		}
		beVectorBase(const beVectorBase&) noexcept = default;
		beVectorBase(beVectorBase&&) noexcept = default;
		beVectorBase& operator=(const beVectorBase&) noexcept = default;
		beVectorBase& operator=(beVectorBase&&) noexcept = default;

		~beVectorBase()
		{
			Release();
		}

		void Release()
		{
			DestructElements(0, m_count);
			Policy::PolicyRelease();
		}

		void Clear()
		{
			DestructElements(0, m_count);
			m_count = 0;
		}
		
		void ReleaseUninitialised()
		{
			Policy::PolicyRelease();
		}

		void Reserve(int capacity)
		{
			Policy::PolicyReserve(capacity);
		}
		
		void ReserveAndSetCountUninitialised(int count)
		{
			Reserve(count);
			SetCountUninitialised(count);
		}

		void SetCount(int count)
		{
			Reserve(count);
			const int currentCount = m_count;
			if (count > currentCount) // growing
			{
				ConstructElements(currentCount, count);
			}
			else if (count < currentCount) // shrinking
			{
				DestructElements(count, currentCount);
			}
			m_count = count;
		}
		
		template <typename... Args>
		void SetCount(int count, Args&&... defaultVal)
		{
			Reserve(count);
			const int currentCount = m_count;
			BE_ASSERT(count <= Capacity());
			if (count > currentCount) // growing
			{
				for (int i = currentCount; i < count; i++)
				{
					// Forward here would be unsafe
					new(&GetBuffer()[i]) value_type(defaultVal...);
				}
			}
			else if (count < currentCount) // shrinking
			{
				DestructElements(count, currentCount);
			}
			m_count = count;
		}

		void SetCountUninitialised(int count)
		{
			Reserve(count);
			m_count = count;
		}

		[[nodiscard]] int Count() const
		{
			return Policy::m_count;
		}
		
		[[nodiscard]] int Capacity() const
		{
			return Policy::PolicyCapacity();
		}
		
		int Insert(T& that)
		{
			if (!CheckRoomForAlloc())
			{
				return -1;
			}

			int index = m_count++;
			BE_NEW(&GetBuffer()[index]) T(that);
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
			DestructElements(index, index+1);
			const int endIndex = m_count - 1;
			const int numToMove = endIndex - index;
			if (numToMove > 0)
			{
				memmove(&GetBuffer()[index], &GetBuffer()[index+1], sizeof(T) * numToMove);
			}
			m_count--;
		}

		void QuickRemove(int index)
		{
			BE_ASSERT(index >= 0 && index < m_count);
			DestructElements(index, index+1);
			const int endIndex = m_count - 1;
			if (index < endIndex)
			{
				memcpy(&GetBuffer()[index], &GetBuffer()[endIndex], sizeof(T));
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
			return (T*)&GetBuffer()[index];
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
			std::uninitialized_copy_n(end() - rangeLength, rangeLength, range);
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
			return GetBuffer();
		}

		T* end()
		{
			return GetBuffer() + m_count;
		}

		const T* cbegin() const
		{
			return GetBuffer();
		}

		const T* cend() const
		{
			return GetBuffer() + m_count;
		}

		const T* begin() const
		{
			return cbegin();
		}

		const T* end() const
		{
			return cend();
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
			return Policy::PolicyCheckRoomForAlloc();
		}

	private:
		using Policy::m_count;

		void ConstructElements(int startElement, int endElement)
		{
			if (!std::is_trivially_constructible_v<T>)
			{
				std::uninitialized_value_construct(&At(startElement), &At(endElement));
			}
		}
		
		void DestructElements(int startElement, int endElement)
		{
			std::destroy(&At(startElement), &At(endElement));
		}
};

template <typename T, int HYBRID_CAPACITY=16>
class beVector : public beVectorBase<T, beVectorHybridPolicy<T, HYBRID_CAPACITY>>
{
	using Base = beVectorBase<T, beVectorHybridPolicy<T, HYBRID_CAPACITY>>;
	public:
	using value_type = typename Base::value_type;
	using iterator = typename Base::iterator;
	using const_iterator = typename Base::const_iterator;

	explicit beVector(int capacity=HYBRID_CAPACITY, int increaseBy=-1) : Base(capacity, increaseBy) {}
	explicit beVector(int capacity, int count, int increaseBy) : Base(capacity, increaseBy) { Base::SetCount(count); }
	beVector(int capacity, int increaseBy, std::initializer_list<T> list) : Base(capacity, increaseBy, list) {}

	~beVector() = default;
	beVector(const beVector&) = delete;
	beVector(beVector&&) = delete;
	beVector& operator=(const beVector&) = delete;
	beVector& operator=(beVector&&) = delete;
};
static_assert(Container<beVector<int>>);

template <typename T, int INITIAL_SIZE=8>
class beHeapVector : public beVectorBase<T, beVectorMallocPolicy<T, INITIAL_SIZE>>
{
	using Base = beVectorBase<T, beVectorMallocPolicy<T>>;
	public:
	using value_type = typename Base::value_type;
	using iterator = typename Base::iterator;
	using const_iterator = typename Base::const_iterator;

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
static_assert(Container<beHeapVector<int>>);

template <typename T, int CAPACITY>
class beFixedVector : public beVectorBase<T, beVectorFixedPolicy<T, CAPACITY>>
{
	using Base = beVectorBase<T, beVectorFixedPolicy<T, CAPACITY>>;
	public:
	using value_type = typename Base::value_type;
	using iterator = typename Base::iterator;
	using const_iterator = typename Base::const_iterator;
	using Base::DataSize;

	beFixedVector() = default;
	beFixedVector(std::initializer_list<T> list) : Base(CAPACITY, 0, list) {}

	~beFixedVector() = default;
	beFixedVector(const beFixedVector&) = delete;
	beFixedVector(beFixedVector&&) = delete;
	beFixedVector& operator=(const beFixedVector&) = delete;
	beFixedVector& operator=(beFixedVector&&) = delete;
};
static_assert(Container<beFixedVector<int, 2>>);
static_assert(Container<beFixedVector<int, 0>>);
#pragma warning(pop)

