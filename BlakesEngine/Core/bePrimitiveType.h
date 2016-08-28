#pragma once

template <typename T>
struct bePrimitiveType
{
	typedef T type;

	T value;

	T operator++() { return ++value; }
	T operator++(int) { return value++; }
};

#define PrimitiveType(name, T, def) struct name : bePrimitiveType<u8>\
{\
	name() : bePrimitiveType{def} {};\
	name(T val) : bePrimitiveType{val} {};\
	bool operator==(const name& that) const {return value == that.value;}\
	bool operator!=(const name& that) const {return value != that.value;}\
}
