#pragma once
#include <string>

typedef std::wstring beWString;
typedef std::string beString;

class beStringView
{
	public:
	beStringView(const beString& beStr) : m_beStr(&beStr), m_cstr(nullptr) {}
	beStringView(const char* cStr) : m_beStr(nullptr), m_cstr(cStr) {}

	int Length() const
	{
		if (m_beStr) { return m_beStr->size(); } 
		return strlen(m_cstr);
	}

	const char* c_str() const
	{
		if (m_beStr) { return m_beStr->c_str(); }
		return m_cstr;
	}

	private:
	const beString* m_beStr;
	const char* m_cstr;
};

namespace beStringConversion
{
	void UTF8ToWide(const beStringView& utf8, beWString* outWStr);
};