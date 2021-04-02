module;
#include "BlakesEngine/Core/beAssert.h"
#include <type_traits>

export module beHashMap;

import beConcepts;
import RangeIter;
import beHash;
import beVector;
import beTypes;

// Dumb first attempt at this, going with 2 parallel vectors for data, one for keys, one for data

export template <typename T, typename U, template <typename, int> class vectorPolicy, int Policy_Size>
class beHashMapBase
{
	public:
	using key_type = T;
	using value_type = U;
	using valid_storage_type = beVectorBase<bool, vectorPolicy<bool, Policy_Size>>;
	using key_storage_type = beVectorBase<key_type, vectorPolicy<key_type, Policy_Size>>;
	using value_storage_type = beVectorBase<value_type, vectorPolicy<value_type, Policy_Size>>;

	beHashMapBase()
	{
		const int capacity = m_validEntries.Capacity();
		m_validEntries.SetCount(capacity, false);
		m_keys.SetCountUninitialised(capacity);
		m_values.SetCountUninitialised(capacity);
	}

	~beHashMapBase()
	{
		Release();
	}

	beHashMapBase(const beHashMapBase&) = default;
	beHashMapBase(beHashMapBase&&) = default;
	beHashMapBase& operator=(const beHashMapBase&) = default;
	beHashMapBase& operator=(beHashMapBase&&) = default;

	void Release()
	{
		for (int i = 0; i < m_validEntries.Count(); i++)
		{
			if (m_validEntries[i])
			{
				m_keys[i].~key_type();
				m_values[i].~value_type();
			}
		}
		m_validEntries.ReleaseUninitialised();
		m_keys.ReleaseUninitialised();
		m_values.ReleaseUninitialised();
	}

	template <typename lookup_type>
	const value_type* Get(const lookup_type& key) const
	{
		int index;
		if (GetHashIndex(key, &index))
		{
			if (m_validEntries[index])
			{
				return &m_values[index];
			}
		}
		return nullptr;
	}
	template <typename lookup_type>
	inline value_type* Get(const lookup_type& key)
	{
		const beHashMapBase* constThis = this;
		return const_cast<value_type*>(constThis->Get(key));
	}
	inline bool Contains(const key_type& key) const { return Get(key) != nullptr; }
	
	template <typename... Args>
	value_type* GetOrCreate(const key_type& key, Args&&... args)
	{
		int insertionIndex;
		if (!GetHashIndex(key, &insertionIndex))
		{
			Grow();
			if (!GetHashIndex(key, &insertionIndex))
			{
				BE_ASSERT(false); // Could not grow successfully!!!
			}
		}

		if (!m_validEntries[insertionIndex])
		{
			m_validEntries[insertionIndex] = true;
			new(&m_keys[insertionIndex]) key_type(key);
			new(&m_values[insertionIndex]) value_type{std::forward<Args>(args)...};
		}
		else
		{
			m_values[insertionIndex] = value_type{std::forward<Args>(args)...};
		}
		return &m_values[insertionIndex];
	}
	
	[[nodiscard]] int Count() const
	{
		return m_validEntries.Count();
	}

	[[nodiscard]] int size() const
	{
		return m_validEntries.Count();
	}

	[[nodiscard]] int Capacity() const
	{
		return m_validEntries.Capacity();
	}
	
	struct iterator
	{
		iterator(beHashMapBase* _map, bool start) : map(_map)
		{
			limit = map->Count();
			if (start)
			{
				index = -1;
				FindNextValid();
			}
			else
			{
				index = limit;
			}
		}
		void FindNextValid()
		{
			if (index == limit)
			{
				return;
			}
			while (++index < limit)
			{
				if (map->m_validEntries[index])
				{
					return;
				}
			}
		}
		beHashMapBase* map;
		int index;
		int limit;
		const key_type& Key() { return map->m_keys[index]; }
		value_type& Value() { return map->m_values[index]; }
		iterator& operator*() { return *this; }
		void operator++() { FindNextValid(); }
		bool operator!=(const iterator& rhs) { return index != rhs.index; }
	};

	struct const_iterator
	{
		const_iterator(beHashMapBase* _map, bool start) : map(_map)
		{
			limit = map->Count();
			if (start)
			{
				index = -1;
				FindNextValid();
			}
			else
			{
				index = limit;
			}
		}
		void FindNextValid()
		{
			if (index == limit)
			{
				return;
			}
			while (++index < limit)
			{
				if (map->m_validEntries[index])
				{
					return;
				}
			}
		}
		const beHashMapBase* map;
		int index;
		int limit;
		const key_type& Key() { return map->m_keys[index]; }
		const value_type& Value() { return map->m_values[index]; }
		const_iterator& operator*() { return *this; }
		void operator++() { FindNextValid(); }
		bool operator!=(const const_iterator& rhs) { return index != rhs.index; }
	};

	iterator begin() { return iterator(this, true); }
	iterator end() { return iterator(this, false); }
	const_iterator cbegin() const { return const_iterator(this, true); }
	const_iterator cend() const { return const_iterator(this, false); }
	const_iterator begin() const { return cbegin(); }
	const_iterator end() const { return cend(); }

	private:
	friend struct iterator;
	friend struct const_iterator;

	template <typename lookup_type>
	bool GetHashIndex(const lookup_type& key, int* out_index) const
	{
		const size_t capacity = Capacity();
		const size_t searchStartIndex = beHash<key_type>::GetHash(key);
		for (size_t i = searchStartIndex, searchEndIndex = searchStartIndex + capacity; i < searchEndIndex; i++)
		{
			int index = (int)(i % capacity);
			if ((!m_validEntries[index]) || (m_keys[index] == key))
			{
				*out_index = index;
				return true;
			}
		}
		return false;
	}

	void Grow()
	{
		if (!m_validEntries.CheckRoomForAlloc())
		{
			BE_ASSERT(false); // Unable to grow
		}
		const int currentCount = m_validEntries.Count();
		const int newCapacity = m_validEntries.Capacity();
		BE_ASSERT(currentCount < newCapacity);

		struct Remap
		{
			Remap(const key_type& key, const value_type& value)
			{
				hash = beHash<key_type>::GetHash(key);
				BE_MEMCPY(keyStorage, &key, sizeof(keyStorage));
				BE_MEMCPY(valueStorage, &value, sizeof(valueStorage));
			}
			size_t hash;
			u8 keyStorage[sizeof(key_type)];
			u8 valueStorage[sizeof(value_type)];
		};
		
		beVector<Remap> remaps(currentCount);
		for (int i = 0; i < currentCount; i++)
		{
			if (m_validEntries[i])
			{
				remaps.AddNew(m_keys[i], m_values[i]);
				m_validEntries[i] = false;
			}
		}
			
		m_validEntries.SetCount(newCapacity, false);
		m_keys.SetCountUninitialised(newCapacity);
		m_values.SetCountUninitialised(newCapacity);

		for (const Remap& remap : remaps)
		{
			const int searchStartIndex = remap.hash % newCapacity;
			for (int i : RangeIter(searchStartIndex, searchStartIndex + newCapacity))
			{
				const int index = i % newCapacity;
				if ((!m_validEntries[index]))
				{
					m_validEntries[index] = true;
					BE_MEMCPY(&m_keys[index], remap.keyStorage, sizeof(key_type));
					BE_MEMCPY(&m_values[index], remap.valueStorage, sizeof(value_type));
					break;
				}
			}
		}
	}

	valid_storage_type m_validEntries;
	key_storage_type m_keys;
	value_storage_type m_values;
};

export template <typename key_type, typename value_type, int HYBRID_CAPACITY=16>
class beHashMap : public beHashMapBase<key_type, value_type, beVectorHybridPolicy, HYBRID_CAPACITY>
{
};

export template <typename key_type, typename value_type, int INITIAL_CAPACITY=16>
class beHashMapHeap : public beHashMapBase<key_type, value_type, beVectorMallocPolicy, INITIAL_CAPACITY>
{
};

export template <typename key_type, typename value_type, int CAPACITY=16>
class beHashMapFixed : public beHashMapBase<key_type, value_type, beVectorFixedPolicy, CAPACITY>
{
};

static_assert(Container<beHashMap<int, int>>);
static_assert(Container<beHashMapHeap<int, int>>);
static_assert(Container<beHashMapFixed<int, int>>);
