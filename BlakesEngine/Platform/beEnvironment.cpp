#include "BlakesEngine/bePCH.h"
#include "beEnvironment.h"

void beEnvironment::Initialise(const beStringView& commandLine)
{
	beString nextKey;
	beString nextValue;
	const char* searchStart = commandLine.c_str();
	bool haveExePath = false;
	bool inQuotes = false;
	bool haveKey = false;

	const char* nextChar = searchStart; 
	while (true)
	{
		bool wordComplete = false;
		if (*nextChar == '\"')
		{
			if (inQuotes)
			{
				wordComplete = true;
			}
			else if (searchStart == nextChar)
			{
				inQuotes = true;
			}
			nextChar++;
		}
		else if (*nextChar == ' ' && !inQuotes)
		{
			if (searchStart == nextChar)
			{
				searchStart++;
				continue;
			}
			wordComplete = true;
		}
		else if (*nextChar == '\0')
		{
			if (inQuotes)
			{
				BE_ASSERT(false); // mismatched quote marks
			}
			wordComplete = true;
		}
		if (wordComplete)
		{
			const char* stringStart = searchStart;
			int stringLen = (int)(nextChar - searchStart);
			if (stringLen == 0)
			{
				// Finished iterating.
				if (haveKey)
				{
					m_map.GetOrCreate(nextKey, "");
				}
				break;
			}
			if (inQuotes)
			{
				stringStart++;
				stringLen -= 2;
				inQuotes = false;
			}
				
			if (!haveExePath)
			{
				m_exePath = beString(stringStart, stringLen);
				haveExePath = true;
			}
			else if (!haveKey)
			{
				haveKey = true;
				BE_ASSERT(*stringStart == '-');
				nextKey = beString(stringStart+1, stringLen-1);
			}
			else
			{
				if (*stringStart == '-')
				{
					m_map.GetOrCreate(nextKey, "");
					nextKey = beString(stringStart+1, stringLen-1);
				}
				else
				{
					m_map.GetOrCreate(nextKey, stringStart, stringLen);
					haveKey = false;
				}
			}
			if (*nextChar != '\0')
			{
				nextChar++;
			}
			searchStart = nextChar;
		}
		else
		{
			 nextChar++;
		}
	}

	LOG("Environment Args:");
	for (auto it : m_map)
	{
		LOG("\t%s = %s", it.Key().c_str(), it.Value().c_str());
	}
}

const beString* beEnvironment::Get(const beString& str)
{
	return m_map.Get(str);
}

const beString* beEnvironment::Get(const char* str)
{
	return m_map.Get(str);
}
