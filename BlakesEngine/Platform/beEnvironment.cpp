module;

#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/beString.h"
#include <fstream>

module beEnvironment;

import beTypes;
import beHash;

static beString RemoveQuotes(const beString& input)
{
	auto inputLen = input.size();
	if (inputLen < 2)
	{
		return input;
	}
	if (input[0] != '"')
	{
		return input;
	}

	BE_ASSERT(input[inputLen-1] == '"');
	return beString(&input[1], inputLen-2);
 }

bool beEnvironment::InitialiseWithFile(const beStringView& filePath)
{
	std::string line;
	std::ifstream fstream(filePath.c_str());
	if (!fstream.is_open())
	{
		return false;
	}
	while (!fstream.eof())
	{
		std::getline(fstream, line);
		if (line.size() == 0)
		{
			continue;
		}
		const char* lineStart = &line[0];
		const char* lineEnd = lineStart + line.size();
		while (*lineStart  == ' ')
		{
			++lineStart;
		}

		if (*lineStart == '#' || *lineStart == ';')
		{
			continue;
		}

		const char* separator = std::find(lineStart, lineEnd, ' ');
		if (separator == lineEnd)
		{
			auto key = RemoveQuotes({lineStart, lineEnd});
			m_map.GetOrCreate(key, "");
		}
		else
		{
			auto key = RemoveQuotes({lineStart, separator});
			auto value = RemoveQuotes({separator+1, lineEnd});
			m_map.GetOrCreate(key, value);
		}
	}
	PrintMap();
	return true;
}

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
			u64 stringLen = (u64)(nextChar - searchStart);
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
	PrintMap();
}

void beEnvironment::PrintMap()
{
	LOG("Environment Args:");
	for (auto it : m_map)
	{
		LOG("\t{} = {}", it.Key().c_str(), it.Value().c_str());
	}
}

const beString* beEnvironment::Get(const beStringView& str)
{
	return m_map.Get(str);
}

const beString* beEnvironment::Get(const char* str)
{
	return m_map.Get(str);
}
