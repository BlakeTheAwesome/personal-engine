module;

#include <concepts>

export module beConcepts;

namespace Private
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

export template<typename T>
concept Container = Private::ContainerImpl<std::remove_cvref_t<T>>;

export template<typename T>
concept Iterator = Private::__LegacyIterator<std::remove_cvref_t<T>>;

export template <class T>
concept Integral = std::is_integral_v<T>;

export template <class T>
concept FloatingPoint = std::is_floating_point_v<T>;

export template <class T>
concept Number = Integral<T> || FloatingPoint<T>;

export template <typename Index>
concept IntOrEnum = std::is_integral_v<Index> || std::is_enum_v<Index>;

export template <typename Fn, typename T>
concept InvokableWith = std::is_invocable_v<Fn, T const&>;