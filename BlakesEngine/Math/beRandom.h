#pragma once
#include "BlakesEngine/Core/beTypes.h"
#include "beMath.h"

// Yoinked from http://www.gamedev.net/topic/643890-simple-and-fast-random-number-generator/
/* Implementation of the Tyche-i RNG by Samuel Neves and Filipe Araujo.
   Period is expected to be 2^127, with a very good uniform distribution. */

class beRandom {
public:
	/* If no seed is passed to the constructor it will use the current time
		in seconds as the seed. */
	beRandom(){};
	beRandom(int seed) 
	{
		Init(seed);
	}

	void Init(int seed)
	{
		a = seed >> 16;
		b = seed << 16;
		c = 2654435769u;
		d = 1367130551u;
	}

	void InitFromSystemTime();

	/* Returns a random integer between 0 and 2^32 - 1, inclusive. */
	u32 Next() {
		using namespace beMath;
		b = rotl32(b, 25) ^ c;
		d = rotl32(d, 24) ^ a;
		c -= d;
		a -= b;
		b = rotl32(b, 20) ^ c;
		d = rotl32(d, 16) ^ a;
		c -= d;
		a -= b;


		return b;
	}


	/* Returns a number between min and max, inclusive. Small statistical
		biases might occur using this method - expected bias is < 2^-32. */
	int Next(int min, int max) {
		if (min > max) {
			u32 t;
			t = min;
			min = max;
			max = t;
		} 


		return min + u32((Next() * u64(max - min + 1)) >> 32);
	}


	/* Returns a random double between 0.0 and 1.0 inclusive. */
	float NextFloat()
	{
		return Next() * (1.f / 4294967296.f);
	}

	float NextFloat(float min, float max)
	{
		float range = max - min;
		return min + (Next() * (range / 4294967296.f));
	}


private:
	u32 a;
	u32 b;
	u32 c;
	u32 d;
};