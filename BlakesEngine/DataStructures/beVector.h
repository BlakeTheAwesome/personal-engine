#pragma once
#include "blakesengine/core/beMacros.h"
#include "blakesengine/core/beAssert.h"

#include <type_traits>
// If increaseBy == -1, double size, if increaseBy == 0, do not increase
template<typename T>
class beVector : public NonCopiable
{
	public:
		typedef T value_type;
		enum { element_size = sizeof(T) };

		explicit beVector(int capacity, int increaseBy=-1)
			: m_buffer(nullptr)
			, m_capacity(0)
			, m_increaseBy(increaseBy)
			, m_count(0)
		{
			Reserve(capacity);
		}
		
		beVector(int capacity, int count, int increaseBy)
			: m_buffer(nullptr)
			, m_capacity(0)
			, m_increaseBy(increaseBy)
			, m_count(0)
		{
			Reserve(capacity);
			SetCount(count);
		}
		
		~beVector()
		{
			Release();
		}
		
		void Release()
		{
			DestructElements(0, m_capacity);
			BE_SAFE_FREE(m_buffer);
			m_count = 0;
		}
		
		void Reserve(int count)
		{
			if (count > m_capacity)
			{
				T* newBuffer = (T*)BE_MALLOC(sizeof(T)*count);
				if (m_count > 0)
				{
					BE_MEMCPY(newBuffer, m_buffer, sizeof(T)*m_count);
					BE_SAFE_FREE(m_buffer);
				}
				m_buffer = newBuffer;
				m_capacity = count;
			}
		}
		
		void ReserveAndSetCount(int count)
		{
			Reserve(count);
			SetCount(count);
		}

		void SetCount(int count)
		{
			BE_ASSERT(count <= m_capacity);
			if (count > m_capacity) // growing
			{
				ConstructElements(m_capacity, count);
			}
			else if (count < m_capacity) // shrinking
			{
				DestructElements(count, m_capacity);
			}
			m_count = count;
		}
		
		int Count() const
		{
			return m_count;
		}
		
		int Insert(T& that)
		{
			if (!CheckRoomForAlloc())
			{
				return -1;
			}

			int index = m_count++;
			m_buffer[index] = that;
			return index;
		}

		T* AllocateNew()
		{
			if (!CheckRoomForAlloc())
			{
				return nullptr;
			}
			int index = m_count++;
			return &m_buffer[index];
		}

		T* AddNew()
		{
			T* obj = AllocateNew();
			obj->T();
			return obj;
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
			return m_buffer[i];
		}

		const T& At(int i) const
		{
			BE_ASSERT(i <= m_count);
			return m_buffer[i];
		}

		T* begin()
		{
			return m_buffer;
		}

		T* end()
		{
			return m_buffer + m_count;
		}

		const T* begin() const
		{
			return m_buffer;
		}

		const T* end() const
		{
			return m_buffer + m_count;
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
				const T* entry = &m_buffer[index];
				
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
		beVector() = delete;

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
			for (int i = startElement; i < endElement; i++)
			{
					m_buffer[i].~T();
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
		
	protected:
		T* m_buffer;
		int m_count;
		int m_capacity;
		int m_increaseBy;
};
