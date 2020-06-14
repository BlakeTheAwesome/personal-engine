#pragma once
#include <concepts>

namespace beConcepts
{
	template<typename T>
	concept ContainerImpl = requires(T cont)
	{
		typename T::value_type;
		typename T::iterator;
		typename T::const_iterator;
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
