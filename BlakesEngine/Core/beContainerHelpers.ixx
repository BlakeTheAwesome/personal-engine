export module beContainerHelpers;

import beConcepts;

export template <Container T>
inline constexpr int ElementSize(T const& container)
{
	return sizeof(typename T::value_type);
}
