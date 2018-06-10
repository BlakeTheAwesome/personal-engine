#pragma once
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/DataStructures/beHashMap.h"

class beEnvironment
{
	public:
	beEnvironment() = default;
	void Initialise(const beStringView& commandLine);

	const beString* Get(const beStringView& str);
	const beString* Get(const char* str);
	private:

	beHashMap<beString, beString> m_map;
	beString m_exePath;
};

