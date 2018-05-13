#pragma once
#include "BlakesEngine/Core/String/StringBuilder.h"
#include <string>

typedef std::wstring beWString;
typedef std::string beString;

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
		const char* start = begin();
		const char* ptr = end();
		while (--ptr >= start)
		{
			if (*ptr == c)
			{
				return (int)std::distance(start, ptr);
			}
		}
		return -1;
	}

	const char* c_str() const
	{
		if (m_cstr) { return m_cstr; }
		if (m_beStr) { return m_beStr->c_str(); }
		return m_ownedStr.c_str();
	}

	const char* begin() const
	{
		return c_str();
	}
	const char* end() const
	{
		return c_str() + Length();
	}
	private:
	const char* m_cstr = nullptr;
	const beString* m_beStr = nullptr;
	const beString m_ownedStr;
};

namespace beStringConversion
{
	void UTF8ToWide(const beStringView& utf8, beWString* outWStr);
	void UTF8FromWide(const beWString& wStr, beString* outStr);
};