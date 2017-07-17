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

	const char* c_str() const
	{
		if (m_cstr) { return m_cstr; }
		if (m_beStr) { return m_beStr->c_str(); }
		return m_ownedStr.c_str();
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