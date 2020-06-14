#pragma once

#if 0
#include <span>
#else
#include "BlakesEngine/External/gsl/span"
namespace std
{
	template <class ElementType, std::size_t Extent=(size_t)-1>
	class span : public gsl::span<ElementType, Extent>
	{
		using gsl::span<ElementType, Extent>::span;
	};

	template <class Type, std::size_t Extent>
	span(Type(&)[Extent])->span<Type, Extent>;

	template <class Type, std::size_t Size>
	span(std::array<Type, Size>&)->span<Type, Size>;

	template <class Type, std::size_t Size>
	span(const std::array<Type, Size>&)->span<const Type, Size>;
}

#endif

#include "beConcepts.h"

template <Container T>
auto to_span(T&& container)
{
	return std::span(container.begin(), container.end());
}

