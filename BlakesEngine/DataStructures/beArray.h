#pragma once
#include "BlakesEngine/Core/beConcepts.h"

#include "beArrayIterator.h"
#include "beConstexprAlgorithms.h"
#include <initializer_list>

using namespace beConstexpr;

template <typename T, auto _Size>
class beArray
{
	static constexpr int Size = (int)_Size;
	public:
	using value_type = T;
	using iterator = T*;
	using const_iterator = const T*;

	constexpr beArray() = default;

	constexpr explicit beArray(const T& defaultValue)
	{
		SetAllTo(defaultValue);
	};

	constexpr beArray(std::initializer_list<T> initialiserList)
	{
		if (initialiserList.size() == 1) // This is so that declaring beArray<int,5>{1} sets all to 1 instead of asserting.
		{
			SetAllTo(*initialiserList.begin());
		}
		else
		{
			BE_ASSERT(initialiserList.size() == Size);
			c_copy(initialiserList.begin(), initialiserList.end(), begin());
		}
	};

	constexpr beArray(gsl::span<const T> span)
	{
		BE_ASSERT(span.size() == Size);
		c_copy(span.begin(), span.end(), begin());
	};

	constexpr inline void SetAllTo(const T& value)
	{
		c_fill(begin(), end(), value);
	}

	// these templates allow you to use enum class entries to index the array
	template <typename E>
	constexpr T& operator[] (E intOrEnum)
	{
		return at((int)intOrEnum);
	}

	template <typename E>
	constexpr T const& operator[] (E intOrEnum) const
	{
		return at((int)intOrEnum);
	}

	constexpr T& First() { return Get(0); }
	constexpr T const& First() const { return Get(0); }
	constexpr T& Last() { return Get(Size - 1); }
	constexpr T const& Last() const { return Get(Size - 1); }

	// Iteration
	constexpr iterator begin()
	{
		return &Get(0);
	}

	constexpr iterator end()
	{
		return &Get(Size);
	}

	constexpr const_iterator cbegin() const
	{
		return &Get(0);
	}

	constexpr const_iterator cend() const
	{
		return &Get(Size);
	}

	constexpr const_iterator begin() const
	{
		return &Get(0);
	}

	constexpr const_iterator end() const
	{
		return &Get(Size);
	}

	constexpr beArrayIter<T>             ArrayIter() { return beArrayIter<T>(begin(), end()); }
	constexpr beArrayIter<const T>       ArrayIter() const { return beArrayIter<const T>(begin(), end()); }
	constexpr beArrayIter<T, true>       ArrayIterReverse() { return beArrayIter<T, true>(begin(), end()); }
	constexpr beArrayIter<const T, true> ArrayIterReverse() const { return beArrayIter<const T, true>(begin(), end()); }
	constexpr beArrayIter<const T>       ArrayIterConst() const { return beArrayIter<const T>(begin(), end()); }
	constexpr beArrayIter<const T, true> ArrayIterConstReverse() const { return beArrayIter<const T, true>(begin(), end()); }

	// Returns the index of the item
	constexpr int IndexOf(const T* t) const
	{
		int index = t - begin();
		BE_ASSERT(index < Size);
		return index;
	}

	// Returns the index of the item in the array, or -1 if not present.
	constexpr int IndexOf(const T& t) const
	{
		for (int i = 0; i < Size; ++i)
		{
			if (Get(i) == t)
			{
				return i;
			}
		}
		return -1;
	}

	// Finding something with a lambda, have to disable template override for things that convert to T (this would get called with const char* searching for const void*)
	template <typename Fn>
	typename std::enable_if_t<!std::is_convertible<Fn, T>::value, int>
	constexpr IndexOf(const Fn& compareFn)
	{
		for (int i = 0; i < Size; ++i)
		{
			if (compareFn(Get(i)))
			{
				return i;
			}
		}
		return -1;
	}

	// Finding something with a lambda, have to disable template override for things that convert to T (this would get called with const char* searching for const void*)
	template <typename Fn>
	typename std::enable_if_t<!std::is_convertible<Fn, T>::value, int>
	constexpr IndexOf(const Fn& compareFn) const
	{
		for (int i = 0; i < Size; ++i)
		{
			if (compareFn(Get(i)))
			{
				return i;
			}
		}
		return -1;
	}

	template <typename U>
	constexpr T* AddressOf(U&& itemOrFn)
	{
		int index = IndexOf(itemOrFn);
		return index != -1 ? &Get(index) : nullptr;
	}

	template <typename U>
	constexpr const T* AddressOf(U&& itemOrFn) const
	{
		int index = IndexOf(itemOrFn);
		return index != -1 ? &Get(index) : nullptr;
	}

	template <typename U>
	constexpr bool Contains(U&& itemOrFn) const
	{
		return IndexOf(itemOrFn) != -1;
	}

	static constexpr int Capacity()
	{
		return Size;
	}

	static constexpr int size()
	{
		return Size;
	}

	// these templates allow you to use enum class entries to index the array
	template <typename E>
	constexpr T& at(E intVal)
	{
		BE_ASSERT(intVal >= 0 && intVal < Size);
		return Get(intVal);
	}

	template <typename E>
	constexpr T const& at(E intVal) const
	{
		BE_ASSERT(intVal >= 0 && intVal < Size);
		return Get(intVal);
	}

	[[nodiscard]] constexpr T* data() { return m_storage; }
	[[nodiscard]] constexpr const void* data() const { return m_storage; }
	[[nodiscard]] constexpr int ByteSize() const { return Size * sizeof(T); }
	void ZeroMem() { memset(m_storage, 0, sizeof(m_storage)); }

	constexpr beArray& operator= (const beArray& rhs)
	{
		if (this != &rhs)
		{
			c_copy(rhs.begin(), rhs.end(), begin());
		}
		return *this;
	}

	constexpr bool operator== (const beArray& rhs) const
	{
		return c_equal(begin(), end(), rhs.begin());
	}

	static constexpr int SIZE = Size;

	// std::span compatibility
	constexpr operator gsl::span<T>()
	{
		return {begin(), end()};
	}
	constexpr operator gsl::span<const T>() const
	{
		return {begin(), end()};
	}
	constexpr operator gsl::span<T, SIZE>()
	{
		return m_storage;
	}
	constexpr operator gsl::span<const T, SIZE>() const
	{
		return m_storage;
	}
	protected:
	constexpr T& Get(int i)
	{
		return m_storage[i];
	}
	constexpr const T& Get(int i) const
	{
		return m_storage[i];
	}
	T m_storage[Size];


};

// If you end up making a zero lengthed fixed size array, allow it to compile.
template <typename T>
class beArray<T, 0>
{
	public:
	using value_type = T;
	using iterator = T*;
	using const_iterator = const T*;
	constexpr beArray() = default;
	constexpr explicit beArray(const T& defaultValue) {}
	constexpr beArray(const T* defaults, int count) {}
	constexpr beArray& operator= (const beArray& rhs) = default;

	constexpr void SetAllTo(const T& value) {}
	constexpr T& operator [] (int i) { return Get(i); }
	constexpr const T& operator [] (int i) const { return Get(i); }
	template <typename E>
	constexpr T& operator [] (E e) { return (*this)[(int)e]; }
	template <typename E>
	constexpr const T& operator [] (E e) const { return (*this)[(int)e]; }

	constexpr T& First() { return Get(0); }
	constexpr const T& First() const { return Get(0); }
	constexpr T& Last() { return Get(0); }
	constexpr const T& Last() const { return Get(0); }
	constexpr iterator begin() { return nullptr; }
	constexpr iterator end() { return nullptr; }
	constexpr const_iterator cbegin() const { return nullptr; }
	constexpr const_iterator cend() const { return nullptr; }
	constexpr const_iterator begin() const { return nullptr; }
	constexpr const_iterator end() const { return nullptr; }
	constexpr beArrayIter<T> ArrayIter() { return beArrayIter<T>(begin(), end()); }
	constexpr beArrayIter<const T> ArrayIter() const { return beArrayIter<const T>(begin(), end()); }
	constexpr beArrayIter<const T> ArrayIterConst() const { return beArrayIter<const T>(begin(), end()); }
	constexpr beArrayIter<T, true> ArrayIterReverse() { return beArrayIter<T, true>(begin(), end()); }
	constexpr beArrayIter<const T, true> ArrayIterReverse() const { return beArrayIter<const T, true>(begin(), end()); }
	constexpr int IndexOf(const T& t) const { return -1; }
	static constexpr int Capacity() { return 0; }
	static constexpr int size() { return 0; }
	constexpr bool Contains(const T& val) const { return false; }
	constexpr T& at(int i) { return Get(i); }
	constexpr const T& at(int i) const { return Get(i); }
	[[nodiscard]] constexpr T* data() { return nullptr; }
	[[nodiscard]] constexpr const void* data() const { return nullptr; }
	[[nodiscard]] constexpr int ByteSize() const { return 0; }
	constexpr void ZeroMem() {}
	static constexpr int SIZE = 0;
	private:
	constexpr T& Get(int i) const { return *(T*)1; }
};

static_assert(Container<beArray<int, 2>>);
static_assert(Container<beArray<int, 0>>);
