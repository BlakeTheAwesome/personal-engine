#include "bePCH.h"
#include "beString.h"
#include "beAssert.h"
#include <Windows.h>

void beStringConversion::UTF8ToWide(const beStringView& utf8, beWString * outWStr)
{
	int strLen = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, utf8.c_str(), -1, nullptr, 0);
	if (strLen < 256)
	{
		wchar_t buffer[256];
		strLen = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, utf8.c_str(), -1, buffer, 256);
		BE_ASSERT(GetLastError() != ERROR_NO_UNICODE_TRANSLATION);
		outWStr->assign(buffer);
		return;
	}

	wchar_t* buffer = BE_NEW wchar_t[strLen];
	strLen = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, utf8.c_str(), -1, buffer, strLen);
	BE_ASSERT(GetLastError() != ERROR_NO_UNICODE_TRANSLATION);
	outWStr->assign(buffer);
	BE_FREE(buffer);
}
