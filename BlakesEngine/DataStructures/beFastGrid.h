#pragma once
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Math/beZOrder.h"

template<typename T, int LENGTH>
class beFastGrid
{
	static const int STRIDE_LENGTH = beMath::NextPowerOfTwo(LENGTH);
	static const int CAPACITY = STRIDE_LENGTH * STRIDE_LENGTH;
	public:
		using value_type = T;
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

		int ToIndex(int x, int y) const
		{
			return beZOrder::EncodeMorton2(x, y);
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
			bool operator==(const iterator& rhs) { return index == rhs.index; }
			bool operator!=(const iterator& rhs) { return !(*this == rhs); }
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
			bool operator==(const const_iterator& rhs) { return index == rhs.index; }
			bool operator!=(const const_iterator& rhs) { return !(*this == rhs); }
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


		struct GridIterator
		{
			beFastGrid* grid;
			struct grid_iterator
			{
				grid_iterator(iterator _it) : it(_it) {}
				iterator it;
				void operator++() { it.index++; }
				bool operator==(const grid_iterator& rhs) { return it == rhs.it; }
				bool operator!=(const grid_iterator& rhs) { return !(*this == rhs); }
				iterator operator*() { return it; }
			};
			grid_iterator begin()
			{
				return iterator(grid, 0);
			}
			grid_iterator end()
			{
				return iterator(grid, CAPACITY);
			}
		};
		GridIterator GridIter() { return GridIterator{this}; }

		struct ConstGridIterator
		{
			beFastGrid* grid;

			struct const_grid_iterator
			{
				const_grid_iterator(const_iterator _it) : it(_it) {}
				const_iterator it;
				void operator++() { it.index++; }
				bool operator==(const const_grid_iterator& rhs) { return it == rhs.it; }
				bool operator!=(const const_grid_iterator& rhs) { return !(*this == rhs); }
				const_iterator operator*() { return it; }
			};

			const_grid_iterator begin() const
			{
				return const_iterator(grid, 0);
			}

			const_grid_iterator end() const
			{
				return const_iterator(grid, CAPACITY);
			}
		};
		ConstGridIterator GridIter() const { return ConstGridIterator{this}; }

	protected:
		T m_buffer[CAPACITY];
};
