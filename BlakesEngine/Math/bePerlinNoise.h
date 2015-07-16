#pragma once
#include "beMath.h"
#include "beRandom.h"
#include "BlakesEngine/DataStructures/beVector.h"

// Currently loops at 256.0, might be better to go for 3d and have z = 0, have to test and see.
class bePerlinNoise2D
{
	public:
	bePerlinNoise2D();
	~bePerlinNoise2D(){}

	void Initialise(int randomSeed);

	float Get(float x, float y);
	float GetOctave(float x, float y, int octaves, float persistence = 0.5f); // Persistence 1 means take full value of every octave, Persistance 0 means only take first.

	private:
	float Grad(int hash, float x, float y);
	beVector<u8> m_hashTable;
};

