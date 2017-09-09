#pragma once
#include <iterator>

namespace RangeIterator
{
	template <typename T>
	struct RangeIter : std::iterator<std::input_iterator_tag, T>
	{
		RangeIter(T val) : val(val) {}
		T operator*() const { return val; }
		const T* operator->() const { return &val; }
		RangeIter& operator ++() { ++val; return *this; }
		RangeIter operator ++(int) { auto copy = *this; ++*this; return copy; }

		bool operator ==(RangeIter const& other) const { return val == other.val; }
		bool operator !=(RangeIter const& other) const { return !(*this == other); }

		private:
		T val;
	};

	template <typename T>
	struct RangeIterReverse : std::iterator<std::input_iterator_tag, T>
	{
		RangeIterReverse(T val) : val(val) {}
		T operator*() const { return val; }
		const T* operator->() const { return &val; }
		RangeIterReverse& operator ++() { --val; return *this; }
		RangeIterReverse operator ++(int) { auto copy = *this; --*this; return copy; }

		bool operator ==(RangeIterReverse const& other) const { return val == other.val; }
		bool operator !=(RangeIterReverse const& other) const { return !(*this == other); }

		private:
		T val;
	};


	template <typename T>
	struct Range
	{
		Range(T start, T end) : m_start(start), m_end(end) {};
		RangeIter<T> begin() const { return RangeIter<T>(m_start); }
		RangeIter<T> end() const { return RangeIter<T>(m_end); }
		T m_start, m_end;
	};

	template <typename T>
	struct RangeReverse
	{
		RangeReverse(T start, T end) : m_start(start), m_end(end) {};
		RangeIterReverse<T> begin() const { return RangeIterReverse<T>(m_end-1); }
		RangeIterReverse<T> end() const { return RangeIterReverse<T>(m_start-1); }
		T m_start, m_end;
	};
}

template <typename T>
auto RangeIter(T start, T end)
{
	static_assert(std::is_integral_v<T>);
	return RangeIterator::Range<T>(start, end);
}

template <typename T>
auto RangeIter(T end)
{
	static_assert(std::is_integral_v<T>);
	return RangeIterator::Range<T>(0, end);
}

template <typename T>
auto RangeIterReverse(T start, T end)
{
	static_assert(std::is_integral_v<T>);
	return RangeIterator::RangeReverse<T>(start, end);
}

template <typename T>
auto RangeIterReverse(T end)
{
	static_assert(std::is_integral_v<T>);
	return RangeIterator::RangeReverse<T>(0, end);
}

