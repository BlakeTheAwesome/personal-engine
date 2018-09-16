#pragma once
#include "BlakesEngine/Core/String/StringBuilder.h"
#include <string>

using beWString = std::wstring;
using beString = std::string;

class beStringBuilder : public ExternalStringBuilder::StringBuilder
{
};

class beStringView
{
	public:
	beStringView(const beStringBuilder& sb) : m_ownedStr(sb.ToString()) {}
	beStringView(const beString& beStr) : m_beStr(&beStr) {}
	beStringView(beString&& beStr) : m_ownedStr(std::move(beStr)) {}
	beStringView(const char* cStr) : m_cstr(cStr) {}

	int Length() const
	{
		if (m_cstr) { return (int)strlen(m_cstr); } 
		if (m_beStr) { return (int)m_beStr->size(); } 
		return (int)m_ownedStr.size();
	}

	gsl::span<const char> ToSpan() const
	{
		return {c_str(), Length()};
	}

	auto begin() const
	{
		return c_str();
	}
	
	auto end() const
	{
		return c_str() + Length();
	}
	auto rbegin() const
	{
		return std::reverse_iterator(end());
	}
	
	auto rend() const
	{
		return std::reverse_iterator(begin());
	}

	int FindFirst(char c) const
	{
		auto _begin = begin();
		auto _end = end();
		auto it = std::find(_begin, _end, c);
		if (it != _end)
		{
			return (int)std::distance(_begin, it);
		}
		return -1;
	}

	int FindLast(char c) const
	{
		const auto _begin = begin();
		const auto _rbegin = rbegin();
		const auto _rend = rend();
		auto it = std::find(_rbegin, _rend, c);
		if (it != _rend)
		{
			return (int)std::distance(&(*_begin), &(*it));
		}
		return -1;
	}

	const char* c_str() const
	{
		if (m_cstr) { return m_cstr; }
		if (m_beStr) { return m_beStr->c_str(); }
		return m_ownedStr.c_str();
	}

	bool IsEqual(const beString& rhs) const
	{
		if (m_beStr) { return rhs == *m_beStr; }
		if (m_cstr) { return rhs == m_cstr; }
		return rhs == m_ownedStr;
	}

	beString ToString() const
	{
		if (m_beStr) { return *m_beStr; }
		if (m_cstr) { return beString(m_cstr); }
		return m_ownedStr;
	}

	private:
	const char* m_cstr = nullptr;
	const beString* m_beStr = nullptr;
	const beString m_ownedStr;
};

inline bool operator==(const beString& lhs, const beStringView& rhs)
{
	return rhs.IsEqual(lhs);
}
inline bool operator==(const beStringView& lhs, const beString& rhs)
{
	return lhs.IsEqual(rhs);
}

namespace fmt
{
	template <>
	struct formatter<beStringView>
	{
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const beStringView &stringView, FormatContext &ctx)
		{
			return format_to(ctx.begin(), "{}", stringView.c_str());
		}
	};
}

namespace beStringConversion
{
	void UTF8ToWide(const beStringView& utf8, beWString* outWStr);
	void UTF8FromWide(const beWString& wStr, beString* outStr);

	beWString UTF8ToWide(const beStringView& utf8);
	beString UTF8FromWide(const beWString& wStr);
};