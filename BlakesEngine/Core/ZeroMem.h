#pragma once
#include <type_traits>

template <typename T>
void ZeroMem(T* obj)
{
	static_assert(!std::is_pointer_v<T>);
	memset(obj, 0, sizeof(T));
}