#pragma once

template <typename T>
inline constexpr int ElementSize(T& container)
{
	return sizeof(typename T::value_type);
}

