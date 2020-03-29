#pragma once
#include <span>

#include "beConcepts.h"

template <Container T>
auto to_span(T&& container)
{
	return std::span(container.begin(), container.end());
}

