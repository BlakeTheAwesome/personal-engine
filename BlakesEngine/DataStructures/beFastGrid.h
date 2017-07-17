#pragma once
#include "blakesengine/core/beAssert.h"
#include "blakesengine/math/beZOrder.h"

template<typename T, int LENGTH>
class beFastGrid
{
	static const int STRIDE_LENGTH = beMath::NextPowerOfTwo(LENGTH);
	static const int CAPACITY = STRIDE_LENGTH * STRIDE_LENGTH;
	public:
		typedef T value_type;
		enum { element_size = sizeof(T) };

		beFastGrid() = default;
		beFastGrid(const T& def) { SetAll(def); }
		beFastGrid(std::initializer_list<T> list)// : m_buffer{list} Todo: fix this
		{
			BE_ASSERT(list.size() == 1 || list.size() == CAPACITY);
			if (list.size() == 1)
			{
				SetAll(*list.begin());
			}
			else
			{
				const T* buf = list.begin();
				for (int i = 0; i < CAPACITY; i++)
				{
					m_buffer[i] = buf[i];
				}
			}
		}
		~beFastGrid() = default;
		
		void SetAll(const T& v)
		{
			for (int i = 0; i < CAPACITY; i++)
			{
				m_buffer[i] = v;
			}
		}
		
		constexpr int Capacity() const
		{
			return CAPACITY;
		}
		
		constexpr int Length() const
		{
			return LENGTH;
		}

		T& operator[](int i)
		{
			return At(i);
		}

		const T& operator[](int i) const
		{
			return At(i);
		}

		T& At(int x, int y)
		{
			return At(beZOrder::EncodeMorton2(x, y));
		}

		const T& At(int x, int y) const
		{
			return At(beZOrder::EncodeMorton2(x, y));
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

		struct iterator
		{
			iterator(beFastGrid* _grid, int _index) : grid(_grid), index(_index) {}
			beFastGrid* grid;
			int index;
			void operator++() { index++; }
			void operator==(const iterator& rhs) { return index == rhs.index; }
			T& operator*() { return grid->At(index); }
			int xPos() const { return beZOrder::DecodeMorton2X(index); }
			int yPos() const { return beZOrder::DecodeMorton2Y(index); }
		};
		struct const_iterator
		{
			const_iterator(beFastGrid* _grid, int _index) : grid(_grid), index(_index) {}
			const beFastGrid* grid;
			int index;
			void operator++() { index++; }
			void operator==(const const_iterator& rhs) { return index == rhs.index; }
			const T& operator*() { return grid->At(index); }
			int xPos() const { return beZOrder::DecodeMorton2X(index); }
			int yPos() const { return beZOrder::DecodeMorton2Y(index); }
		};

		iterator begin()
		{
			return iterator(this, 0);
		}

		iterator end()
		{
			return iterator(this, CAPACITY);
		}
		
		const_iterator begin() const
		{
			return const_iterator(this, 0);
		}

		const_iterator end() const
		{
			return const_iterator(this, CAPACITY);
		}

	protected:
		T m_buffer[CAPACITY];
};
