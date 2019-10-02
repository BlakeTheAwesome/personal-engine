#pragma once
#include "BlakesEngine/Core/beRanges.h"

template <typename T>
auto RangeIter(T start, T end)
{
	static_assert(std::is_integral_v<T>);
	return std::views::iota(start, end);
}

template <typename T>
auto RangeIter(T end)
{
	static_assert(std::is_integral_v<T>);
	return std::views::iota(T{}, end);
}

template <typename T>
auto RangeIterReverse(T start, T end)
{
	static_assert(std::is_integral_v<T>);
	return std::views::reverse(std::views::iota(start, end));
}

template <typename T>
auto RangeIterReverse(T end)
{
	static_assert(std::is_integral_v<T>);
	return std::views::iota(T{}, end) | std::views::reverse;
}

