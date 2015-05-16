#ifndef _beVector_h_
#define _beVector_h_

#include "blakesengine/core/beMacros.h"

// If increaseBy == -1, double size, if increaseBy == 0, do not increase
template<typename T>
class beVector
{
	public:
		beVector(int count, int increaseBy=-1)
			: m_buffer(NULL)
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
			if (m_count >= m_bufferLength)
			{
				switch (m_increaseBy)
				{
					case 0:
					{
						BE_ASSERT(false);
						return -1;
					}
					case -1:
					{
						Reserve(m_bufferLength * 2);
						break;
					}
					default:
					{
						Reserve(m_bufferLength + m_increaseBy);
						break;
					}
				}
			}
			int index = m_count++;
			m_buffer[index] = that;
			return index;
		}
		
	private:
		beVector();
		beVector(const beVector&);
		beVector& operator= (const beVector&);
		
	protected:
		T* m_buffer;
		int m_count;
		int m_bufferLength;
		int m_increaseBy;
};

#endif //_beVector_h_

