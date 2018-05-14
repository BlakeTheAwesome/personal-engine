#pragma once
#include "beString.h"
#include <locale>

namespace beStringUtil
{
	inline std::locale s_locale;

	#define WRAP_PERMUTATIONS_CHAR(FunctionName)\
		inline auto FunctionName(const beStringView& str, char c) { return FunctionName(str.begin(), str.end(), c); }\
		inline auto FunctionName(const beWString& str, char c)    { return FunctionName(str.begin(), str.end(), c); }\
	// WRAP_PERMUTATIONS_CHAR


	#define WRAP_PERMUTATIONS(FunctionName)\
		template <typename Iter> inline auto FunctionName(Iter beginRange, Iter endRange, const beStringView& pattern) { return FunctionName(beginRange, endRange, pattern.begin(), pattern.end()); }\
		                         inline auto FunctionName(const beStringView& str, const beStringView& pattern)        { return FunctionName(str.begin(), str.end(), pattern.begin(), pattern.end()); }\
		template <typename Iter> inline auto FunctionName(Iter beginRange, Iter endRange, const beWString& pattern)    { return FunctionName(beginRange, endRange, pattern.begin(), pattern.end()); }\
		                         inline auto FunctionName(const beWString& str, const beWString& pattern)              { return FunctionName(str.begin(), str.end(), pattern.begin(), pattern.end()); }\
		                         inline auto FunctionName(const beWString& str, const wchar_t* pattern)                { return FunctionName(str.begin(), str.end(), pattern, pattern + wcslen(pattern)); }\
	// WRAP_PERMUTATIONS

	template <typename Iter>
	inline int FindFirst(Iter begin, Iter end, char c)
	{
		auto it = std::find(begin, end, c);
		if (it != end)
		{
			return (int)std::distance(begin, it);
		}
		return -1;
	}
	WRAP_PERMUTATIONS_CHAR(FindFirst);

	template <typename Iter, typename IterPattern>
	inline int FindFirst(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		if (std::distance(beginRange, endRange) < std::distance(beginPattern, endPattern))
		{
			// range too small, cannot contain string
			return -1;
		}

		auto it = std::search(beginRange, endRange, beginPattern, endPattern);
		if (it != endRange)
		{
			return (int)std::distance(beginRange, it);
		}
		return -1;
	}
	WRAP_PERMUTATIONS(FindFirst);



	template <typename Iter>
	inline int FindLast(Iter begin, Iter end, char c)
	{
		while (--end >= begin)
		{
			if (*end == c)
			{
				return (int)std::distance(begin, end);
			}
		}
		return -1;
	}
	WRAP_PERMUTATIONS_CHAR(FindLast);

	template <typename Iter, typename IterPattern>
	inline int FindLast(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		if (std::distance(beginRange, endRange) < std::distance(beginPattern, endPattern))
		{
			// range too small, cannot contain string
			return -1;
		}

		auto it = std::find_end(beginRange, endRange, beginPattern, endPattern);
		if (it != endRange)
		{
			return (int)std::distance(beginRange, it);
		}
		return -1;
	}
	WRAP_PERMUTATIONS(FindLast);

	template <typename Iter, typename IterPattern>
	inline bool IsEqual(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		if (std::distance(beginRange, endRange) != std::distance(beginPattern, endPattern))
		{
			return false;
		}
		return FindFirst(beginRange, endRange, beginPattern, endPattern) == 0;
	}
	WRAP_PERMUTATIONS(IsEqual);

	template <typename Iter>
	struct LowerIterator : public std::iterator_traits<Iter>
	{
		LowerIterator(const Iter& iter) : iter(iter) {}
		Iter iter;
		auto operator*() const { return std::tolower(*iter, s_locale); }
		LowerIterator& operator++() { ++iter; return *this; }
		auto operator-  (const LowerIterator& val) const { return iter -  val.iter; }
		auto operator<  (const LowerIterator& val) const { return iter <  val.iter; }
		auto operator>  (const LowerIterator& val) const { return iter >  val.iter; }
		auto operator<= (const LowerIterator& val) const { return iter <= val.iter; }
		auto operator>= (const LowerIterator& val) const { return iter >= val.iter; }
		auto operator== (const LowerIterator& val) const { return iter == val.iter; }
		auto operator!= (const LowerIterator& val) const { return iter != val.iter; }
		template <typename T> auto operator+  (T val) const { return LowerIterator(iter + val); }
		template <typename T> auto operator+= (T val) { iter += val; return *this; }
		template <typename T> auto operator-  (T val) const { return LowerIterator(iter - val); }
		template <typename T> auto operator-= (T val) { iter -= val; return *this; }
		template <typename T> auto operator[] (T val) const { return iter[val]; }
	};

	template <typename Iter, typename IterPattern>
	inline bool IsEqualI(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		if (std::distance(beginRange, endRange) != std::distance(beginPattern, endPattern))
		{
			return false;
		}

		return IsEqual(LowerIterator{beginRange}, LowerIterator{endRange}, LowerIterator{beginPattern}, LowerIterator{endPattern});
	}
	WRAP_PERMUTATIONS(IsEqualI);

	#undef WRAP_PERMUTATIONS
	#undef WRAP_PERMUTATIONS_CHAR
};
