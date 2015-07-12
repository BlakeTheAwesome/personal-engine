#include "bePCH.h"
#include "bePerlinNoise.h"

bePerlinNoise2D::bePerlinNoise2D()
	: m_maxX(0)
	, m_maxY(0)
	, m_precomputed(false)
	, m_precomputedGradients(0, -1)
{
}

void bePerlinNoise2D::Initialise(int maxX, int maxY, int randomSeed)
{
	BE_ASSERT(m_precomputedGradients.Count() == 0);
	m_maxX = maxX;
	m_maxY = maxY;
	m_randomSeed = randomSeed;
}

void bePerlinNoise2D::Precompute()
{
	int seed = m_randomSeed;
	int numEntries = m_maxX * m_maxY;
	m_precomputedGradients.ReserveAndSetCount(numEntries);

	beRandom rng;
	for (int i = 0; i < numEntries; i++)
	{
		rng.Init(seed + i);
		m_precomputedGradients[i] = Vec2(rng.NextFloat(-1.f, 1.f), rng.NextFloat(-1.f, 1.f));
	}
}

float bePerlinNoise2D::Get(float x, float y)
{
	Vec2 g00, g01, g10, g11;
	float xOffset, yOffset;
	GetGradients(x, y, &g00, &g01, &g10, &g11, &xOffset, &yOffset);

	Vec2 g00Offset(xOffset, yOffset);
	Vec2 g01Offset(xOffset, 1.f-yOffset);
	Vec2 g10Offset(1.f-xOffset, yOffset);
	Vec2 g11Offset(1.f-xOffset, 1.f-yOffset);

	float val00 = Dot(g00, g00Offset);
	float val01 = Dot(g01, g01Offset);
	float val10 = Dot(g10, g10Offset);
	float val11 = Dot(g11, g11Offset);

	float val0 = SmoothLerp(val00, val01, xOffset);
	float val1 = SmoothLerp(val10, val11, xOffset);

	float val = SmoothLerp(val0, val1, yOffset);

	return val;
}

void bePerlinNoise2D::GetGradients(float x, float y, Vec2* g00, Vec2* g01, Vec2* g10, Vec2* g11, float* out_xOffset, float* out_yOffset)
{
	double xIntPart, xFractPart, yIntPart, yFractPart;
	xFractPart = modf(x, &xIntPart);
	yFractPart = modf(y, &yIntPart);

	if (xFractPart < 0.0)
	{
		xIntPart -= 1.0;
		xFractPart += 1.0;
	}
	if (yFractPart < 0.0)
	{
		yIntPart -= 1.0;
		yFractPart += 1.0;
	}

	int x0 = (int)xIntPart;
	int y0 = (int)yIntPart;
	int maxX = m_maxX;
	int maxY = m_maxY;

	x0 = ((x0 % maxX) + maxX) % maxX; // [0, maxX)
	y0 = ((y0 % maxY) + maxY) % maxY; // [0, maxX)
	int x1 = (x0 + 1) % maxX; // [0, maxX)
	int y1 = (y0 + 1) % maxY; // [0, maxX)
	
	*out_xOffset = (float)xFractPart;
	*out_yOffset = (float)yFractPart;

	int index00 = x0 + (y0*maxX);
	int index01 = x1 + (y0*maxX);
	int index10 = x0 + (y1*maxX);
	int index11 = x1 + (y1*maxX);

	if (m_precomputedGradients.Count() > 0)
	{
		*g00 = m_precomputedGradients[index00];
		*g01 = m_precomputedGradients[index01];
		*g10 = m_precomputedGradients[index10];
		*g11 = m_precomputedGradients[index11];
	}
	else
	{
		int seed = m_randomSeed;

		beRandom rng;
		rng.Init(seed + index00);
		*g00 = Vec2(rng.NextFloat(-1.f, 1.f), rng.NextFloat(-1.f, 1.f));
		
		rng.Init(seed + index01);
		*g01 = Vec2(rng.NextFloat(-1.f, 1.f), rng.NextFloat(-1.f, 1.f));
		
		rng.Init(seed + index10);
		*g10 = Vec2(rng.NextFloat(-1.f, 1.f), rng.NextFloat(-1.f, 1.f));
		
		rng.Init(seed + index11);
		*g11 = Vec2(rng.NextFloat(-1.f, 1.f), rng.NextFloat(-1.f, 1.f));
	}
}
