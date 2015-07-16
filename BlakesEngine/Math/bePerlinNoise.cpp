#include "bePCH.h"
#include "bePerlinNoise.h"

#include <random> 
#include <algorithm> 

// First used original Perlin noise implementation from: http://webstaff.itn.liu.se/~stegu/TNM022-2005/perlinnoiselinks/perlin-noise-math-faq.html
// Updated with improved Perlin noise, found at: http://flafla2.github.io/2014/08/09/perlinnoise.html

bePerlinNoise2D::bePerlinNoise2D()
	: m_hashTable(256, 0)
{
}

void bePerlinNoise2D::Initialise(int randomSeed)
{
	BE_ASSERT(m_hashTable.Count() == 0);

	m_hashTable.ReserveAndSetCount(256);
	for (int i = 0; i < 256; i++)
	{
		m_hashTable[i] = (u8)i;
	}

	std::shuffle(m_hashTable.begin(), m_hashTable.end(), std::default_random_engine(randomSeed));
}

float bePerlinNoise2D::Get(float x, float y)
{
	float xi, xf, yi, yf;
	xf = modf(x, &xi);
	yf = modf(y, &yi);

	if (xf < 0.0)
	{
		xi -= 1.0;
		xf += 1.0;
	}
	if (yf < 0.0)
	{
		yi -= 1.0;
		yf += 1.0;
	}

	int x0 = (int)xi;
	int y0 = (int)yi;
	int maxX = 255;
	int maxY = 255;

	x0 = ((x0 % maxX) + maxX) % maxX; // [0, maxX)
	y0 = ((y0 % maxY) + maxY) % maxY; // [0, maxX)
	int x1 = (x0 + 1) % maxX; // [0, maxX)
	int y1 = (y0 + 1) % maxY; // [0, maxX)
	
	float u = beMath::SmootherStep(xf);
	float v = beMath::SmootherStep(yf);

	BE_ASSERT(m_hashTable.Count());

	u8* p = m_hashTable.begin();
	int aa, ab, ba, bb;
	aa = p[p[x0]+y0];
	ab = p[p[x0]+y1];
	ba = p[p[x1]+y0];
	bb = p[p[x1]+y1];
	
	// The gradient function calculates the dot product between a pseudorandom
	// gradient vector and the vector from the input coordinate to the 8
	// surrounding points in its unit cube.
	// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
	// values we made earlier.

	float aaba = beMath::Lerp(Grad(aa, xf, yf)  , Grad(ba, xf-1, yf)  , u);
	float abbb = beMath::Lerp(Grad(ab, xf, yf-1), Grad(bb, xf-1, yf-1), u);
	float result = beMath::Lerp(aaba, abbb, v);
		
	// Change (-1,1) => (0, 1)
	return (result+1.f) * 0.5f;
}

float bePerlinNoise2D::Grad(int hash, float x, float y)
{
	switch(hash & 0xF)
	{
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x;     // + z;
		case 0x5: return -x;     // + z;
		case 0x6: return  x;     // - z;
		case 0x7: return -x;     // - z;
		case 0x8: return  y;     // + z;
		case 0x9: return -y;     // + z;
		case 0xA: return  y;     // - z;
		case 0xB: return -y;     // - z;
		case 0xC: return  y + x;
		case 0xD: return -y;     // + z;
		case 0xE: return  y - x;
		case 0xF: return -y;     // - z;
		default: break; // never happens
	}
	return 0; // impossibru
}
