#include "BlakesEngine/bePCH.h"
#include "beString.h"
#include "beAssert.h"
#include "BlakesEngine/Platform/beWindows.h"

void beStringConversion::UTF8ToWide(const beStringView& utf8, beWString * outWStr)
{
	int strLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	if (strLen < 256)
	{
		wchar_t buffer[256];
		buffer[0] = 0;
		strLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, buffer, 256);
		BE_ASSERT(GetLastError() != ERROR_NO_UNICODE_TRANSLATION);
		outWStr->assign(buffer);
		return;
	}

	wchar_t* buffer = BE_NEW wchar_t[strLen];
	strLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, buffer, strLen);
	BE_ASSERT(GetLastError() != ERROR_NO_UNICODE_TRANSLATION);
	outWStr->assign(buffer);
	BE_FREE(buffer);
}

void beStringConversion::UTF8FromWide(const beWString& wStr, beString* outStr)
{
	int strLen = (int)wStr.size();
	int requiredBufferSize = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), strLen, nullptr, 0, nullptr, nullptr);

	if (requiredBufferSize < 256)
	{
		char buffer[256];
		buffer[0] = 0;
		int len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), strLen, buffer, requiredBufferSize, nullptr, nullptr);
		BE_ASSERT(GetLastError() != ERROR_NO_UNICODE_TRANSLATION);
		outStr->assign(buffer, len);
		return;
	}

	char* buffer = BE_NEW char[requiredBufferSize];
	buffer[0] = 0;
	int len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), strLen, buffer, requiredBufferSize, nullptr, nullptr);
	BE_ASSERT(GetLastError() != ERROR_NO_UNICODE_TRANSLATION);
	outStr->assign(buffer, len);
	BE_FREE(buffer);
}


