#pragma once
#include "beMath.h"
#include "beRandom.h"
#include "BlakesEngine/DataStructures/beVector.h"

// Calculates 1 gradient at each whole number, if you need higher fidelity, then we need to scale either here or there.
class bePerlinNoise2D
{
	public:
	bePerlinNoise2D();
	~bePerlinNoise2D(){}

	void Initialise(int maxX, int maxY, int randomSeed);
	void Precompute();

	float Get(float x, float y);

	private:
	void GetGradients(float x, float y, Vec2* g00, Vec2* g01, Vec2* g10, Vec2* g11, float* xOffset, float* yOffset); // x and y offset are values between 0 and 1 for lerping

	beVector<Vec2> m_precomputedGradients;
	int m_randomSeed;
	int m_maxX;
	int m_maxY;
	bool m_precomputed;
};

