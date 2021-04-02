#pragma once
import beConcepts;


template <Container T>
inline constexpr int ElementSize(T const& container)
{
	return sizeof(typename T::value_type);
}

