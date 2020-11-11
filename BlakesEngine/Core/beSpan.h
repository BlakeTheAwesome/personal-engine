#pragma once

#if 1
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

	template<class It, class End>
	span(It first, End last)->span<std::remove_reference_t<decltype(*first)>>;
}

#endif

#include "beConcepts.h"

namespace beSpan::detail
{
	template<auto> struct require_constant;
	template<class T>
	concept has_constexpr_size = requires { typename require_constant<T::size()>; };
}

template <Container T>
auto to_span(T&& container)
{

	if constexpr (beSpan::detail::has_constexpr_size<T>)
	{
		return std::span<typename T::value_type, container.size()>(container.begin(), container.end());
	}
	else
	{
		return std::span(container.begin(), container.end());
	}
}

