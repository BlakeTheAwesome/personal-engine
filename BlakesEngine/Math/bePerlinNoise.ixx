export module bePerlinNoise;

import beRandom;
import beMath;
import beArray;
import beTypes;

// Currently loops at 256.0, might be better to go for 3d and have z = 0, have to test and see.
export class bePerlinNoise2D
{
	public:
	void Initialise(int randomSeed);

	float Get(float x, float y);
	float GetOctave(float x, float y, int octaves, float persistence = 0.5f); // Persistence 1 means take full value of every octave, Persistance 0 means only take first.

	private:
	float Grad(int hash, float x, float y, float z);
	beArray<u8, 512> m_hashTable;
};

