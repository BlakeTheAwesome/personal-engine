#include "bePCH.h"
#include "bePrintf.h"

#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>

void bePrintf::bePrintf(bool includeLocation, const char* location, const char* format, ...)
{
	char formatBuffer[2048];

	sprintf_s(formatBuffer, 2048, "%s: %s", location, format);

	char buffer[4096];
	const size_t maxChars = sizeof(buffer)-3;

	va_list args;
	va_start(args, format);
	int length = vsnprintf_s(buffer, maxChars, maxChars, includeLocation ? formatBuffer : format, args);
	va_end(args);

	if (length < 0)
	{
		// We have killed our buffer or something, bail.
		return;
	}
	
	char* p = buffer + length;
	*p++ = '\r';
	*p++ = '\n';
	*p   = '\0';

	OutputDebugStringA(buffer);
}
