#pragma once
#include "BlakesEngine/Core/beConcepts.h"


template <Container T>
inline constexpr int ElementSize(T const& container)
{
	return sizeof(typename T::value_type);
}

