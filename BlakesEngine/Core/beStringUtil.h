#pragma once
#include "beString.h"
import beConcepts;

#include <locale>

namespace beStringUtil
{
	inline std::locale s_locale; 

	#define WRAP_PERMUTATIONS_CHAR(FunctionName)\
		inline auto FunctionName(const beStringView& str, char c) { return FunctionName(str.begin(), str.end(), c); }\
		inline auto FunctionName(const beWString& str, char c)    { return FunctionName(str.begin(), str.end(), c); }\
	// WRAP_PERMUTATIONS_CHAR


	#define WRAP_PERMUTATIONS(FunctionName)\
		template <Iterator Iter> inline auto FunctionName(Iter beginRange, Iter endRange, const beStringView& pattern) { return FunctionName(beginRange, endRange, pattern.begin(), pattern.end()); }\
		                         inline auto FunctionName(const beStringView& str, const beStringView& pattern)        { return FunctionName(str.begin(), str.end(), pattern.begin(), pattern.end()); }\
		template <Iterator Iter> inline auto FunctionName(Iter beginRange, Iter endRange, const beWString& pattern)    { return FunctionName(beginRange, endRange, pattern.begin(), pattern.end()); }\
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

	template <Iterator Iter, Iterator IterPattern>
	inline bool BeginsWith(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		if (std::distance(beginRange, endRange) < std::distance(beginPattern, endPattern))
		{
			return false;
		}
		#pragma warning(push)
		#pragma warning(disable:26459) // Passing raw pointer iterator
		return std::equal(beginPattern, endPattern, beginRange);
		#pragma warning(pop)
	}
	WRAP_PERMUTATIONS(BeginsWith);

	template <Iterator Iter, Iterator IterPattern>
	inline bool BeginsWithI(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		return BeginsWith(LowerIterator{beginRange}, LowerIterator{endRange}, LowerIterator{beginPattern}, LowerIterator{endPattern});
	}
	WRAP_PERMUTATIONS(BeginsWithI);

	template <Iterator Iter, Iterator IterPattern>
	inline bool EndsWith(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		const auto rbeginRange = std::reverse_iterator(endRange);
		const auto rendRange = std::reverse_iterator(beginRange);
		const auto rbeginPattern = std::reverse_iterator(endPattern);
		const auto rendPattern = std::reverse_iterator(beginPattern);
		return BeginsWith(rbeginRange, rendRange, rbeginPattern, rendPattern);
	}
	WRAP_PERMUTATIONS(EndsWith);

	template <Iterator Iter, Iterator IterPattern>
	inline bool EndsWithI(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		const auto rbeginRange = std::reverse_iterator(endRange);
		const auto rendRange = std::reverse_iterator(beginRange);
		const auto rbeginPattern = std::reverse_iterator(endPattern);
		const auto rendPattern = std::reverse_iterator(beginPattern);
		return BeginsWithI(rbeginRange, rendRange, rbeginPattern, rendPattern);
	}
	WRAP_PERMUTATIONS(EndsWithI);


	template <Iterator Iter, Iterator IterPattern>
	inline bool IsEqual(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		if (std::distance(beginRange, endRange) != std::distance(beginPattern, endPattern))
		{
			return false;
		}
		return FindFirst(beginRange, endRange, beginPattern, endPattern) == 0;
	}
	WRAP_PERMUTATIONS(IsEqual);
	
	template <Iterator Iter, Iterator IterPattern>
	inline bool IsEqualI(Iter beginRange, Iter endRange, IterPattern beginPattern, IterPattern endPattern)
	{
		return IsEqual(LowerIterator{beginRange}, LowerIterator{endRange}, LowerIterator{beginPattern}, LowerIterator{endPattern});
	}
	WRAP_PERMUTATIONS(IsEqualI);

	#undef WRAP_PERMUTATIONS
	#undef WRAP_PERMUTATIONS_CHAR
};
