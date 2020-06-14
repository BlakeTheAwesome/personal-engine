#pragma once
#include <concepts>

namespace beConcepts
{
	// from https://en.cppreference.com/w/cpp/named_req/Iterator
	template<class T>
	concept __LegacyIterator = std::copyable<T> && requires(T i)
	{
		{ *i };
		{ ++i } -> std::same_as<T&>;
		{ *i++ };
	};

	template<typename T>
	concept ContainerImpl = requires(T cont)
	{
		typename T::value_type;
		typename T::iterator;
		typename T::const_iterator;
		__LegacyIterator<typename T::value_type>;
		{ cont.begin() }->std::same_as<typename T::iterator>;
		{ cont.end() }->std::same_as<typename T::iterator>;
		{ std::declval<const T>().begin() }->std::same_as<typename T::const_iterator>;
		{ std::declval<const T>().end() }->std::same_as<typename T::const_iterator>;
		{ cont.cbegin() }->std::same_as<typename T::const_iterator>;
		{ cont.cend() }->std::same_as<typename T::const_iterator>;
		{ cont.size() }->std::convertible_to<size_t>;
	};

}

template<typename T>
concept Container = beConcepts::ContainerImpl<std::remove_cvref_t<T>>;

template<typename T>
concept Iterator = beConcepts::__LegacyIterator<std::remove_cvref_t<T>>;

template <class T>
concept Integral = std::is_integral<T>::value;

template <typename Index>
concept IntOrEnum = std::is_integral_v<Index> || std::is_enum_v<Index>;

template <typename Fn, typename T>
concept InvokableWith = std::is_invocable_v<Fn, T const&>;