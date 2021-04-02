#pragma once
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Math/beZOrder.h"
#include "BlakesEngine/DataStructures/beArray.h"

import beConcepts;
import beMath;

template<typename T, int LENGTH>
class beFastGrid
{
	static const int STRIDE_LENGTH = NextPowerOfTwo(LENGTH);
	static const int CAPACITY = STRIDE_LENGTH * STRIDE_LENGTH;
	public:
		using value_type = T;
		enum { element_size = sizeof(T) };

		beFastGrid() = default;
		beFastGrid(const T& def) { SetAll(def); }
		beFastGrid(std::initializer_list<T> list)
			: m_buffer(list)
		{
		}
		~beFastGrid() = default;
		
		void SetAll(const T& v)
		{
			m_buffer.SetAllTo(v);
		}
		
		[[nodiscard]] constexpr int Capacity() const
		{
			return CAPACITY;
		}
		
		[[nodiscard]] constexpr int Length() const
		{
			return LENGTH;
		}

		[[nodiscard]] constexpr int size() const
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
			bool operator==(const iterator& rhs) const { return index == rhs.index; }
			bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
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
			bool operator==(const const_iterator& rhs) const { return index == rhs.index; }
			bool operator!=(const const_iterator& rhs) const { return !(*this == rhs); }
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

		const_iterator cbegin() const
		{
			return const_iterator(this, 0);
		}

		const_iterator cend() const
		{
			return const_iterator(this, CAPACITY);
		}
		
		const_iterator begin() const
		{
			return cbegin();
		}

		const_iterator end() const
		{
			return cend();
		}


		struct GridIterator
		{
			beFastGrid* grid;
			struct grid_iterator
			{
				grid_iterator(iterator _it) : it(_it) {}
				iterator it;
				void operator++() { it.index++; }
				bool operator==(const grid_iterator& rhs) const { return it == rhs.it; }
				bool operator!=(const grid_iterator& rhs) const { return !(*this == rhs); }
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
				bool operator==(const const_grid_iterator& rhs) const { return it == rhs.it; }
				bool operator!=(const const_grid_iterator& rhs) const { return !(*this == rhs); }
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
		beArray<T, CAPACITY> m_buffer;
};

static_assert(Container<beFastGrid<int, 16>>);
