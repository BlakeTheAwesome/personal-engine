#pragma once
#include "BlakesEngine/Core/beRanges.h"
#include <concepts>

template <std::integral T>
auto RangeIter(T start, T end)
{
	static_assert(std::is_integral_v<T>);
	return nano::views::iota(start, end);
}

template <std::integral T>
auto RangeIter(T end)
{
	return RangeIter(T{}, end);
}

template <std::integral T>
auto RangeIterReverse(T start, T end)
{
	auto range = RangeIter(start, end);
	return range | nano::views::reverse;
}

template <std::integral T>
auto RangeIterReverse(T end)
{
	return RangeIterReverse(T{}, end);
}

