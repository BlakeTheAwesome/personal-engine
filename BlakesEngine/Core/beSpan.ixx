module;

#include <span>
export module beSpan;

import beConcepts;

namespace beSpan::detail
{
	template<auto> struct require_constant;
	template<class T>
	concept has_constexpr_size = requires { typename require_constant<T::size()>; };
}

export template <Container T>
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

