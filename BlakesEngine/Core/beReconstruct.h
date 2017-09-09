#pragma once

template <typename T, typename ...Args>
void Reconstruct(T* self, Args&&... args)
{
	if (!std::is_trivially_destructible_v<T>)
	{
		self->~T();
	}
	new(self) T(std::forward<Args>(args)...);
}