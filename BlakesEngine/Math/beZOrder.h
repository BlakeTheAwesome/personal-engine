#pragma once
#include "BlakesEngine/Core/beTypes.h"
#include "BlakesEngine/Math/beMath.h"

// z order curves/morton curves give nice cache locality when dealing with 2d arrays (used in texture swizzling)

namespace beZOrder
{
	// "Insert" a 0 bit after each of the 16 low bits of x
	constexpr u32 Part1By1(u32 x)
	{
		x &= 0x0000ffff;                  // x = ---- ---- ---- ---- fedc ba98 7654 3210
		x = (x | (x <<  8)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
		x = (x | (x <<  4)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
		x = (x | (x <<  2)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
		x = (x | (x <<  1)) & 0x55555555; // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
		return x;
	}
	
	// "Insert" two 0 bits after each of the 10 low bits of x
	constexpr u32 Part1By2(u32 x)
	{
		x &= 0x000003ff;                  // x = ---- ---- ---- ---- ---- --98 7654 3210
		x = (x | (x << 16)) & 0xff0000ff; // x = ---- --98 ---- ---- ---- ---- 7654 3210
		x = (x | (x <<  8)) & 0x0300f00f; // x = ---- --98 ---- ---- 7654 ---- ---- 3210
		x = (x | (x <<  4)) & 0x030c30c3; // x = ---- --98 ---- 76-- --54 ---- 32-- --10
		x = (x | (x <<  2)) & 0x09249249; // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
		return x;
	}

	// Inverse of Part1By1 - "delete" all odd-indexed bits
	constexpr u32 Compact1By1(u32 x)
	{
		x &= 0x55555555;                  // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
		x = (x | (x >>  1)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
		x = (x | (x >>  2)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
		x = (x | (x >>  4)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
		x = (x | (x >>  8)) & 0x0000ffff; // x = ---- ---- ---- ---- fedc ba98 7654 3210
		return x;
	}

	// Inverse of Part1By2 - "delete" all bits not at positions divisible by 3
	constexpr u32 Compact1By2(u32 x)
	{
		x &= 0x09249249;                  // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
		x = (x | (x >>  2)) & 0x030c30c3; // x = ---- --98 ---- 76-- --54 ---- 32-- --10
		x = (x | (x >>  4)) & 0x0300f00f; // x = ---- --98 ---- ---- 7654 ---- ---- 3210
		x = (x | (x >>  8)) & 0xff0000ff; // x = ---- --98 ---- ---- ---- ---- 7654 3210
		x = (x | (x >> 16)) & 0x000003ff; // x = ---- ---- ---- ---- ---- --98 7654 3210
		return x;
	}

	
	constexpr u32 EncodeMorton2(u32 x, u32 y)
	{
		return (Part1By1(y) << 1) + Part1By1(x);
	}

	constexpr u32 EncodeMorton3(u32 x, u32 y, u32 z)
	{
		return (Part1By2(z) << 2) + (Part1By2(y) << 1) + Part1By2(x);
	}
	constexpr u32 DecodeMorton2X(u32 code)
	{
		return Compact1By1(code >> 0);
	}

	constexpr u32 DecodeMorton2Y(u32 code)
	{
		return Compact1By1(code >> 1);
	}

	constexpr u32 DecodeMorton3X(u32 code)
	{
		return Compact1By2(code >> 0);
	}

	constexpr u32 DecodeMorton3Y(u32 code)
	{
		return Compact1By2(code >> 1);
	}

	constexpr u32 DecodeMorton3Z(u32 code)
	{
		return Compact1By2(code >> 2);
	}
}
