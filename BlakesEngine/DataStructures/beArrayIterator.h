#pragma once
#include "BlakesEngine/Core/beGSL.h"

template <typename T, bool reverse=false>
class beArrayIter
{
	public:
	constexpr beArrayIter(gsl::span<T> span)
		: m_itStart(span.data())
		, m_itEnd(span.data() + span.size())
		, m_count((int)span.size())
	{
	}

	struct iterator
	{
		constexpr iterator(int index, T* ptr)
			: m_index(index)
			, m_ptr(ptr)
		{
		}

		constexpr inline void operator++()
		{
			if (reverse)
			{
				--m_index;
				--m_ptr;
			}
			else
			{
				++m_index;
				++m_ptr;
			}
		}

		constexpr inline bool operator!=(const iterator& rhs) const
		{
			return m_index != rhs.m_index;
		}

		struct value_type
		{
			constexpr value_type(int _index, T& _value) : index(_index), value(_value) {}
			int index;
			T& value;
		};
		constexpr inline value_type operator*() { return value_type{m_index, *m_ptr}; }

		T* m_ptr = nullptr;
		int m_index = 0;
	};

	constexpr inline iterator begin()
	{
		if (reverse)
		{
			return iterator {m_count-1, m_itEnd-1};
		}
		else
		{
			return iterator { 0, m_itStart };
		}
	}

	constexpr inline iterator end()
	{
		if (reverse)
		{
			return iterator {-1, m_itStart-1};
		}
		else
		{
			return iterator {m_count, m_itEnd};
		}
	}

	private:
	T* m_itStart = nullptr;
	T* m_itEnd = nullptr;
	int m_count = 0;
};

template<typename T> beArrayIter(gsl::span<T>) -> beArrayIter<T>;
