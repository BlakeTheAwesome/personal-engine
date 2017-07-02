#pragma once
#include <type_traits>

template <typename T, typename... Args>
void Reconstruct(T* obj, Args&&... args)
{
	if (!std::is_trivially_destructible_v<T>)
	{
		obj->~T();
	}
	new(obj) T(std::forward<Args>(args)...);
}
