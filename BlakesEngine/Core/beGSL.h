#pragma once
#include "gsl/gsl"

template <typename T>
auto to_span(T&& container)
{
	return gsl::span(container.begin(), container.end());
}