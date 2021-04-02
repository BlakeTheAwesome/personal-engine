module;
#include "BlakesEngine/Core/beString.h"

export module beEnvironment;

import beHashMap;

export class beEnvironment
{
	public:
	bool InitialiseWithFile(const beStringView& filePath);
	void Initialise(const beStringView& commandLine);

	void PrintMap();

	const beString* Get(const beStringView& str);
	const beString* Get(const char* str);
	private:

	beHashMap<beString, beString> m_map;
	beString m_exePath;
};

