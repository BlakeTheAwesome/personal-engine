#include "bePCH.h"
#include "bePerlinNoise.h"

#include <random> 
#include <algorithm> 

// First used original Perlin noise implementation from: http://webstaff.itn.liu.se/~stegu/TNM022-2005/perlinnoiselinks/perlin-noise-math-faq.html
// Updated with improved Perlin noise, found at: http://flafla2.github.io/2014/08/09/perlinnoise.html

bePerlinNoise2D::bePerlinNoise2D()
	: m_hashTable(512, 0)
{
}

void bePerlinNoise2D::Initialise(int randomSeed)
{
	BE_ASSERT(m_hashTable.Count() == 0);

	u8 hashTable[256];
	for (int i = 0; i < 256; i++)
	{
		hashTable[i] = (u8)i;
	}
	std::shuffle(hashTable, hashTable + 256, std::default_random_engine(randomSeed));

	// We have this at double the size so that the p[p[p[x0]+y0]+z0]; code doesn't run off the end (could also mask it with 0xff at each step).
	m_hashTable.ReserveAndSetCount(512);
	for (int i = 0; i < 512; i++)
	{
		m_hashTable[i] = hashTable[i & 0xff];
	}
}

float bePerlinNoise2D::GetOctave(float x, float y, int octaves, float persistence)
{
	float total = 0.f;
	float frequency = 1.f;
	float amplitude = 1.f;
	float maxValue = 0.f; // Used for normalizing result to [0.0,1.0]

	for (int i=0; i<octaves; i++)
	{
		total += Get(x * frequency, y * frequency) * amplitude;
			
		maxValue += amplitude;
			
		amplitude *= persistence;
		frequency *= 2.f;
	}
		
	return total/maxValue;

}

float bePerlinNoise2D::Get(float x, float y)
{
	float xi, xf, yi, yf;
	xf = modf(x, &xi);
	yf = modf(y, &yi);
	float zf = 0.5f;

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
	float w = beMath::SmootherStep(zf);

	BE_ASSERT(m_hashTable.Count());

	u8* p = m_hashTable.begin();
	
	int z0 = 0;
	int z1 = 1;

	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	aaa = p[p[p[x0]+y0]+z0];
	aba = p[p[p[x0]+y1]+z0];
	aab = p[p[p[x0]+y0]+z1];
	abb = p[p[p[x0]+y1]+z1];
	baa = p[p[p[x1]+y0]+z0];
	bba = p[p[p[x1]+y1]+z0];
	bab = p[p[p[x1]+y0]+z1];
	bbb = p[p[p[x1]+y1]+z1];
	
	// The gradient function calculates the dot product between a pseudorandom
	// gradient vector and the vector from the input coordinate to the 8
	// surrounding points in its unit cube.
	// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
	// values we made earlier.


	float X1, X2, Y1, Y2;
	X1 = Lerp(Grad(aaa, xf, yf    , zf)    , Grad(baa, xf-1.f, yf    , zf)    , u);
	X2 = Lerp(Grad(aba, xf, yf-1.f, zf)    , Grad(bba, xf-1.f, yf-1.f, zf)    , u);
	Y1 = Lerp(X1, X2, v);
 
	X1 = Lerp(Grad(aab, xf, yf    , zf-1.f), Grad(bab, xf-1.f, yf    , zf-1.f), u);
	X2 = Lerp(Grad(abb, xf, yf-1.f, zf-1.f), Grad(bbb, xf-1.f, yf-1.f, zf-1.f), u);
	Y2 = Lerp(X1, X2, v);
	
	// Change (-1,1) => (0, 1)
	float result = (Lerp(Y1, Y2, w)+1.f) * 0.5f;
	//bePRINTF("X,Y,Z {%3.1f, %3.1f, %3.1f}\t = %3.3f", x, y, zf, result);
	return result;
}

float bePerlinNoise2D::Grad(int hash, float x, float y, float z)
{
	switch(hash & 0xF)
	{
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;
		case 0x8: return  y + z;
		case 0x9: return -y + z;
		case 0xA: return  y - z;
		case 0xB: return -y - z;
		case 0xC: return  y + x;
		case 0xD: return -y + z;
		case 0xE: return  y - x;
		case 0xF: return -y - z;
		default: break; // never happens
	}
	return 0; // impossibru
}
