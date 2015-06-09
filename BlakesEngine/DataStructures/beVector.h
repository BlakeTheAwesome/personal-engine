#pragma once
#include "blakesengine/core/beMacros.h"
#include "blakesengine/core/beAssert.h"

// If increaseBy == -1, double size, if increaseBy == 0, do not increase
template<typename T>
class beVector
{
	public:
		beVector(int count, int increaseBy=-1)
			: m_buffer(nullptr)
			, m_bufferLength(0)
			, m_increaseBy(increaseBy)
			, m_count(0)
		{
			Reserve(count);
		}
		
		~beVector()
		{
			Release();
		}
		
		void Release()
		{
			BE_SAFE_FREE(m_buffer);
			m_count = 0;
		}
		
		void Reserve(int length)
		{
			if (length > m_bufferLength)
			{
				T* newBuffer = (T*)BE_MALLOC(sizeof(T)*length);
				if (m_count > 0)
				{
					BE_MEMCPY(newBuffer, m_buffer, sizeof(T)*m_count);
					BE_SAFE_FREE(m_buffer);
				}
				m_buffer = newBuffer;
				m_bufferLength = length;
			}
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
		beVector(const beVector&) = delete;
		beVector& operator= (const beVector&) = delete;

		bool CheckRoomForAlloc()
		{
			if (m_count >= m_bufferLength)
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
						Reserve(m_bufferLength * 2);
						return true;
					}
					default:
					{
						Reserve(m_bufferLength + m_increaseBy);
						return true;
					}
				}
			}
			return true;
		}
		
	protected:
		T* m_buffer;
		int m_count;
		int m_bufferLength;
		int m_increaseBy;
};
