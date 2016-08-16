#pragma once
#include "blakesengine/core/beAssert.h"

template<typename T, int CAPACITY>
class beArray
{
	public:
		typedef T value_type;
		enum { element_size = sizeof(T) };

		beArray() = default;
		beArray(const std::initializer_list<T>& list) : m_buffer{list} {}
		~beArray() = default;
		
		void SetAll(const T& v)
		{
			for (int i = 0; i < CAPACITY; i++)
			{
				m_buffer[i] = v;
			}
		}
		
		int Capacity() const
		{
			return CAPACITY;
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
			BE_ASSERT(i <= CAPACITY);
			return m_buffer[i];
		}

		const T& At(int i) const
		{
			BE_ASSERT(i <= CAPACITY);
			return m_buffer[i];
		}

		T* begin()
		{
			return m_buffer;
		}

		T* end()
		{
			return m_buffer + CAPACITY;
		}

		const T* begin() const
		{
			return m_buffer;
		}

		const T* end() const
		{
			return m_buffer + CAPACITY;
		}

	protected:
		T m_buffer[CAPACITY];
};
