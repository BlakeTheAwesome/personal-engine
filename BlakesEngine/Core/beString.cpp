#include "BlakesEngine/bePCH.h"
#include "beString.h"
#include "beAssert.h"
#include "BlakesEngine/Platform/beWindows.h"
#include "BlakesEngine/DataStructures/beVector.h"

void beStringConversion::UTF8ToWide(const beStringView& utf8, beWString* outWStr)
{
	int strLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	beVector<wchar_t, 256> buffer;
	buffer.SetCount(strLen);
	buffer[0] = 0;
	strLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, buffer.begin(), buffer.Count());
	BE_ASSERT(GetLastError() != ERROR_NO_UNICODE_TRANSLATION);
	outWStr->assign(buffer.begin());
}

void beStringConversion::UTF8FromWide(const beWString& wStr, beString* outStr)
{
	const int strLen = (int)wStr.size();
	const int requiredBufferSize = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), strLen, nullptr, 0, nullptr, nullptr);

	beVector<char, 256> buffer;
	buffer[0] = 0;
	const int len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), strLen, buffer.begin(), requiredBufferSize, nullptr, nullptr);
	BE_ASSERT(GetLastError() != ERROR_NO_UNICODE_TRANSLATION);
	outStr->assign(buffer.begin(), len);
	return;
}

beWString beStringConversion::UTF8ToWide(const beStringView& utf8)
{
	beWString ret;
	UTF8ToWide(utf8, &ret);
	return ret;
}

beString beStringConversion::UTF8FromWide(const beWString& wStr)
{
	beString ret;
	UTF8FromWide(wStr, &ret);
	return ret;
}

