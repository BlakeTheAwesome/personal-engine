#pragma once
#include <functional>
#include "BlakesEngine/Core/beString.h"

namespace beHashFunctions
{
	u32 MurmerHash32(const void* data, int dataLen);
}

template <typename T>
struct beHash
{
	static size_t GetHash(const T& obj)
	{
		return std::hash<T>{}(obj);
	}
};

template <>
struct beHash<beString>
{
	static size_t GetHash(const char* str)
	{
		return beHashFunctions::MurmerHash32(str, (int)strlen(str));
	}
	
	static size_t GetHash(const beString& str)
	{
		return beHashFunctions::MurmerHash32(str.c_str(), (int)str.size());
	}
};

