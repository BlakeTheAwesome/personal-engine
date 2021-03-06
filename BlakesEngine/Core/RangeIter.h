#pragma once
#include "BlakesEngine/Core/beRanges.h"
#include <concepts>

template <std::integral T>
auto RangeIter(T start, T end)
{
	static_assert(std::is_integral_v<T>);
	return std::views::iota(start, end);
}

template <std::integral T>
auto RangeIter(T end)
{
	static_assert(std::is_integral_v<T>);
	return std::views::iota(T{}, end);
}

template <std::integral T>
auto RangeIterReverse(T start, T end)
{
	static_assert(std::is_integral_v<T>);
	return std::views::reverse(std::views::iota(start, end));
}

template <std::integral T>
auto RangeIterReverse(T end)
{
	static_assert(std::is_integral_v<T>);
	return std::views::iota(T{}, end) | std::views::reverse;
}

