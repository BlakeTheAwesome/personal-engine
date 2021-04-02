#pragma once
#include <concepts>

template <std::integral T>
struct bePrimitiveType
{
	using type = T;

	T value;

	T operator++() { return ++value; }
	const T operator++(int) { return value++; }
};

#define PrimitiveType(name, T, def) struct name : bePrimitiveType<u8>\
{\
	name() : bePrimitiveType{def} {};\
	name(T val) : bePrimitiveType{val} {};\
	bool operator==(const name& that) const {return value == that.value;}\
	bool operator!=(const name& that) const {return value != that.value;}\
}
