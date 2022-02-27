#pragma once
#include "beString.h"
import beConcepts;

#include <locale>
#include <algorithm>

namespace beStringUtil
{
	inline std::locale s_locale; 

	#define WRAP_PERMUTATIONS_CHAR(FunctionName)\
		inline auto FunctionName(const beStringView& str, char c) { return FunctionName(str.begin(), str.end(), c); }\
		inline auto FunctionName(const beWString& str, char c)    { return FunctionName(str.begin(), str.end(), c); }\
	// WRAP_PERMUTATIONS_CHAR


	#define WRAP_PERMUTATIONS(FunctionName)\
		template <Iterator Iter> inline auto FunctionName(Iter beginHaystack, Iter endHaystack, const beStringView& pattern) { return FunctionName(beginHaystack, endHaystack, pattern.begin(), pattern.end()); }\
		                         inline auto FunctionName(const beStringView& str, const beStringView& pattern)        { return FunctionName(str.begin(), str.end(), pattern.begin(), pattern.end()); }\
		template <Iterator Iter> inline auto FunctionName(Iter beginHaystack, Iter endHaystack, const beWString& pattern)    { return FunctionName(beginHaystack, endHaystack, pattern.begin(), pattern.end()); }\
		                         inline auto FunctionName(const beWString& str, const beWString& pattern)              { return FunctionName(str.begin(), str.end(), pattern.begin(), pattern.end()); }\
		                         inline auto FunctionName(const beWString& str, std::span<const wchar_t> pattern)      { return FunctionName(str.begin(), str.end(), pattern.begin(), pattern.end()); }\
	// WRAP_PERMUTATIONS
	#pragma warning(push)
	#pragma warning(disable:26481) // Pointer arithmetic
	template <Iterator Iter>
	struct LowerIterator : public std::iterator_traits<Iter>
	{
		LowerIterator(const Iter& iter) : iter(iter) {}
		Iter iter;
		auto operator*() const { return std::tolower(*iter, s_locale); }
		LowerIterator& operator++() { ++iter; return *this; }
		LowerIterator operator++(int) { auto ret = *this; ++(*this); return ret; }
		LowerIterator& operator--() { --iter; return *this; }
		LowerIterator operator--(int) { auto ret = *this; --(*this); return ret; }
		auto operator-  (const LowerIterator& val) const { return iter -  val.iter; }
		auto operator<  (const LowerIterator& val) const { return iter <  val.iter; }
		auto operator>  (const LowerIterator& val) const { return iter >  val.iter; }
		auto operator<= (const LowerIterator& val) const { return iter <= val.iter; }
		auto operator>= (const LowerIterator& val) const { return iter >= val.iter; }
		auto operator== (const LowerIterator& val) const { return iter == val.iter; }
		auto operator!= (const LowerIterator& val) const { return iter != val.iter; }
		template <Integral T> auto operator+  (T val) const { return LowerIterator(iter + val); }
		template <Integral T> auto operator+= (T val) { iter += val; return *this; }
		template <Integral T> auto operator-  (T val) const { return LowerIterator(iter - val); }
		template <Integral T> auto operator-= (T val) { iter -= val; return *this; }
		template <Integral T> auto operator[] (T val) const { return iter[val]; }
	};
	#pragma warning(pop)


	template <Iterator Iter>
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

	template <Iterator Iter, Iterator IterPattern>
	inline int FindFirst(Iter beginHaystack, Iter endHaystack, IterPattern beginNeedle, IterPattern endNeedle)
	{
		const auto haystackSize = std::distance(beginHaystack, endHaystack);
		const auto needleSize = std::distance(beginNeedle, endNeedle);

		if (haystackSize < needleSize)
		{
			// range too small, cannot contain string
			return -1;
		}
		else if (needleSize == 0)
		{
			return -1;
		}

		auto it = std::search(beginHaystack, endHaystack, beginNeedle, endNeedle);
		if (it != endHaystack)
		{
			return (int)std::distance(beginHaystack, it);
		}
		return -1;
	}
	WRAP_PERMUTATIONS(FindFirst);



	template <Iterator Iter>
	inline int FindLast(Iter begin, Iter end, char c)
	{
		const auto rbegin = std::reverse_iterator(end);
		const auto rend = std::reverse_iterator(begin);
		const auto it = std::find(rbegin, rend, c);
		if (it != rend)
		{
			return (int)std::distance(&(*begin), &(*it));
		}
		return -1;
	}
	WRAP_PERMUTATIONS_CHAR(FindLast);

	template <Iterator Iter, Iterator IterPattern>
	inline int FindLast(Iter beginHaystack, Iter endHaystack, IterPattern beginNeedle, IterPattern endNeedle)
	{
		const auto haystackSize = std::distance(beginHaystack, endHaystack);
		const auto needleSize = std::distance(beginNeedle, endNeedle);
		if (haystackSize < needleSize)
		{
			// range too small, cannot contain string
			return -1;
		}
		else if (needleSize == 0)
		{
			return -1;
		}

		auto it = std::find_end(beginHaystack, endHaystack, beginNeedle, endNeedle);
		if (it != endHaystack)
		{
			return (int)std::distance(beginHaystack, it);
		}
		return -1;
	}
	WRAP_PERMUTATIONS(FindLast);

	template <Iterator Iter, Iterator IterPattern>
	inline bool BeginsWith(Iter beginHaystack, Iter endHaystack, IterPattern beginNeedle, IterPattern endNeedle)
	{
		const auto haystackSize = std::distance(beginHaystack, endHaystack);
		const auto needleSize = std::distance(beginNeedle, endNeedle);
		if (haystackSize < needleSize)
		{
			return false;
		}
		#pragma warning(push)
		#pragma warning(disable:26459) // Passing raw pointer iterator
		return std::equal(beginNeedle, endNeedle, beginHaystack);
		#pragma warning(pop)
	}
	WRAP_PERMUTATIONS(BeginsWith);

	template <Iterator Iter, Iterator IterPattern>
	inline bool BeginsWithI(Iter beginHaystack, Iter endHaystack, IterPattern beginNeedle, IterPattern endNeedle)
	{
		return BeginsWith(LowerIterator{beginHaystack}, LowerIterator{endHaystack}, LowerIterator{beginNeedle}, LowerIterator{endNeedle});
	}
	WRAP_PERMUTATIONS(BeginsWithI);

	template <Iterator Iter, Iterator IterPattern>
	inline bool EndsWith(Iter beginHaystack, Iter endHaystack, IterPattern beginNeedle, IterPattern endNeedle)
	{
		const auto rbeginHaystack = std::reverse_iterator(endHaystack);
		const auto rendHaystack = std::reverse_iterator(beginHaystack);
		const auto rbeginNeedle = std::reverse_iterator(endNeedle);
		const auto rendNeedle = std::reverse_iterator(beginNeedle);
		return BeginsWith(rbeginHaystack, rendHaystack, rbeginNeedle, rendNeedle);
	}
	WRAP_PERMUTATIONS(EndsWith);

	template <Iterator Iter, Iterator IterPattern>
	inline bool EndsWithI(Iter beginHaystack, Iter endHaystack, IterPattern beginNeedle, IterPattern endNeedle)
	{
		const auto rbeginHaystack = std::reverse_iterator(endHaystack);
		const auto rendHaystack = std::reverse_iterator(beginHaystack);
		const auto rbeginNeedle = std::reverse_iterator(endNeedle);
		const auto rendNeedle = std::reverse_iterator(beginNeedle);
		return BeginsWithI(rbeginHaystack, rendHaystack, rbeginNeedle, rendNeedle);
	}
	WRAP_PERMUTATIONS(EndsWithI);


	template <Iterator Iter, Iterator IterPattern>
	inline bool IsEqual(Iter beginHaystack, Iter endHaystack, IterPattern beginNeedle, IterPattern endNeedle)
	{
		const auto haystackSize = std::distance(beginHaystack, endHaystack);
		const auto needleSize = std::distance(beginNeedle, endNeedle);
		if (haystackSize != needleSize)
		{
			return false;
		}
		else if (haystackSize == 0)
		{
			return true;
		}
		return FindFirst(beginHaystack, endHaystack, beginNeedle, endNeedle) == 0;
	}
	WRAP_PERMUTATIONS(IsEqual);
	
	template <Iterator Iter, Iterator IterPattern>
	inline bool IsEqualI(Iter beginHaystack, Iter endHaystack, IterPattern beginNeedle, IterPattern endNeedle)
	{
		return IsEqual(LowerIterator{beginHaystack}, LowerIterator{endHaystack}, LowerIterator{beginNeedle}, LowerIterator{endNeedle});
	}
	WRAP_PERMUTATIONS(IsEqualI);

	#undef WRAP_PERMUTATIONS
	#undef WRAP_PERMUTATIONS_CHAR
};
