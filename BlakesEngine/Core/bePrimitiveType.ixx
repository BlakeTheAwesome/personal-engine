module;
#include <compare>

export module bePrimitiveType;

export template <typename T, T DefaultVal>
struct bePrimitiveType
{
	constexpr bePrimitiveType() = default;
	constexpr bePrimitiveType(T val) : value(val) {}

	constexpr auto operator<=>(bePrimitiveType const&) const = default;

	using type = T;

	T value = DefaultVal;

	T operator++() { return ++value; }
	const T operator++(int) { return value++; }
};
