#pragma once

namespace beConstexpr
{
	template<class InputIterator, class OutputIterator>
	constexpr OutputIterator c_copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		while (first!=last)
		{
			*result = *first;
			++result; ++first;
		}
		return result;
	}

	template <class ForwardIterator, class T>
	constexpr void c_fill(ForwardIterator first, ForwardIterator last, const T& val)
	{
		while (first != last)
		{
			*first = val;
			++first;
		}
	}

	template <class InputIterator1, class InputIterator2>
	constexpr bool c_equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
	{
		while (first1!=last1)
		{
			if (!(*first1 == *first2))
				return false;
			++first1; ++first2;
		}
		return true;
	}

	template <class InputIterator1, class InputIterator2, class BinaryPredicate>
	constexpr bool c_equal(InputIterator1 first1, InputIterator1 last1,
						   InputIterator2 first2, BinaryPredicate pred)
	{
		while (first1!=last1)
		{
			if (!pred(*first1,*first2))
				return false;
			++first1; ++first2;
		}
		return true;
	}
}
