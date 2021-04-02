module;
#include "BlakesEngine/Core/bePrintf.h"
#include "../Core/beAssert.h"
#include "DirectXMath.h"
#include <concepts>

export module beMath;
import beDirectXMath;
import RangeIter;

using namespace DirectXMath;

export
{
	constexpr float PI = 3.141592654f;
	constexpr float RAD_TO_DEG(float rad) { return rad * (180.0f / PI); }
	constexpr float DEG_TO_RAD(float deg) { return deg * (PI / 180.0f); }
	
	using Vec2 = XMFLOAT2;
	using Vec3 = XMFLOAT3;
	using Vec4 = XMFLOAT4;
	using Matrix = XMFLOAT4X4;
	using Quat = XMFLOAT4;

	inline Vec2 V20()  { return Vec2( 0.f,  0.f); }
	inline Vec2 V21()  { return Vec2( 1.f,  1.f); }
	inline Vec2 V2X()  { return Vec2( 1.f,  0.f); }
	inline Vec2 V2Y()  { return Vec2( 0.f,  1.f); }
	inline Vec2 V2N1() { return Vec2(-1.f, -1.f); }
	inline Vec2 V2NX() { return Vec2(-1.f,  0.f); }
	inline Vec2 V2NY() { return Vec2( 0.f, -1.f); }

	inline Vec3 V30()  { return Vec3( 0.f,  0.f,  0.f); }
	inline Vec3 V31()  { return Vec3( 1.f,  1.f,  1.f); }
	inline Vec3 V3X()  { return Vec3( 1.f,  0.f,  0.f); }
	inline Vec3 V3Y()  { return Vec3( 0.f,  1.f,  0.f); }
	inline Vec3 V3Z()  { return Vec3( 0.f,  0.f,  1.f); }
	inline Vec3 V3N1() { return Vec3(-1.f, -1.f, -1.f); }
	inline Vec3 V3NX() { return Vec3(-1.f,  0.f,  0.f); }
	inline Vec3 V3NY() { return Vec3( 0.f, -1.f,  0.f); }
	inline Vec3 V3NZ() { return Vec3( 0.f,  0.f, -1.f); }

	inline Vec4 V40()   { return Vec4( 0.f,  0.f,  0.f,  0.f); }
	inline Vec4 V41()   { return Vec4( 1.f,  1.f,  1.f,  1.f); }
	inline Vec4 V4X()   { return Vec4( 1.f,  0.f,  0.f,  0.f); }
	inline Vec4 V4Y()   { return Vec4( 0.f,  1.f,  0.f,  0.f); }
	inline Vec4 V4Z()   { return Vec4( 0.f,  0.f,  1.f,  0.f); }
	inline Vec4 V4W()   { return Vec4( 0.f,  0.f,  0.f,  1.f); }
	inline Vec4 V4N1()  { return Vec4(-1.f, -1.f, -1.f, -1.f); }
	inline Vec4 V4NX()  { return Vec4(-1.f,  0.f,  0.f,  0.f); }
	inline Vec4 V4NY()  { return Vec4( 0.f, -1.f,  0.f,  0.f); }
	inline Vec4 V4NZ()  { return Vec4( 0.f,  0.f, -1.f,  0.f); }
	inline Vec4 V4XW()  { return Vec4( 1.f,  0.f,  0.f,  1.f); }
	inline Vec4 V4YW()  { return Vec4( 0.f,  1.f,  0.f,  1.f); }
	inline Vec4 V4ZW()  { return Vec4( 0.f,  0.f,  1.f,  1.f); }
	inline Vec4 V4NXW() { return Vec4(-1.f,  0.f,  0.f,  1.f); }
	inline Vec4 V4NYW() { return Vec4( 0.f, -1.f,  0.f,  1.f); }
	inline Vec4 V4NZW() { return Vec4( 0.f,  0.f, -1.f,  1.f); }
	inline Vec4 V4XY()  { return Vec4( 1.f,  1.f,  0.f,  0.f); }
	inline Vec4 V4XZ()  { return Vec4( 1.f,  0.f,  1.f,  0.f); }
	inline Vec4 V4YZ()  { return Vec4( 0.f,  1.f,  1.f,  0.f); }
	inline Vec4 V4XYW() { return Vec4( 1.f,  1.f,  0.f,  1.f); }
	inline Vec4 V4XZW() { return Vec4( 1.f,  0.f,  1.f,  1.f); }
	inline Vec4 V4YZW() { return Vec4( 0.f,  1.f,  1.f,  1.f); }

	inline float Length(const Vec3& v)
	{
		const XMVECTOR xv = XMLoadFloat3(&v);
		XMFLOAT4A out; XMStoreFloat4A(&out, xv);
		return out.x;
	}

	inline Vec4 ToVec4(const Vec3& v, float w)
	{
		return Vec4(v.x, v.y, v.z, w);
	}

	inline Matrix IdentityMatrix()
	{
		return Matrix(1.f, 0.f, 0.f, 0.f,
					  0.f, 1.f, 0.f, 0.f,
					  0.f, 0.f, 1.f, 0.f,
					  0.f, 0.f, 0.f, 1.f);
	}

	inline Vec2& operator*=(Vec2& lhs, const Vec2& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		return lhs;
	}

	inline Vec3& operator*=(Vec3& lhs, const Vec3& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		lhs.z *= rhs.z;
		return lhs;
	}

	inline Vec4& operator*=(Vec4& lhs, const Vec4& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		lhs.z *= rhs.z;
		lhs.w *= rhs.w;
		return lhs;
	}

	inline Vec2 operator*(const Vec2& lhs, const Vec2& rhs)
	{
		return {
			lhs.x * rhs.x,
			lhs.y * rhs.y,
		};
	}

	inline Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
	{
		return {
			lhs.x * rhs.x,
			lhs.y * rhs.y,
			lhs.z * rhs.z,
		};
	}

	inline Vec4 operator*(const Vec4& lhs, const Vec4& rhs)
	{
		return {
			lhs.x * rhs.x,
			lhs.y * rhs.y,
			lhs.z * rhs.z,
			lhs.w * rhs.w,
		};
	}

	inline Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
	{
		const XMVECTOR l = XMLoadFloat3(&lhs);
		const XMVECTOR r = XMLoadFloat3(&rhs);
		const XMVECTOR xv = XMVectorSubtract(l, r);
		Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
	{
		const XMVECTOR l = XMLoadFloat3(&lhs);
		const XMVECTOR r = XMLoadFloat3(&rhs);
		const XMVECTOR xv = XMVectorAdd(l, r);
		Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline Vec3 operator*(const Vec3& v, const float& scale)
	{
		const XMVECTOR l = XMLoadFloat3(&v);
		const XMVECTOR xv = XMVectorScale(l, scale);
		Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline Vec4 operator-(const Vec4& lhs, const Vec4& rhs)
	{
		const XMVECTOR l = XMLoadFloat4(&lhs);
		const XMVECTOR r = XMLoadFloat4(&rhs);
		const XMVECTOR xv = XMVectorSubtract(l, r);
		Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	inline Vec4 operator+(const Vec4& lhs, const Vec4& rhs)
	{
		const XMVECTOR l = XMLoadFloat4(&lhs);
		const XMVECTOR r = XMLoadFloat4(&rhs);
		const XMVECTOR xv = XMVectorAdd(l, r);
		Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	inline Vec4 operator*(const Vec4& v, const float& scale)
	{
		const XMVECTOR l = XMLoadFloat4(&v);
		const XMVECTOR xv = XMVectorScale(l, scale);
		Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	inline bool operator==(const Matrix& m1, const Matrix& m2)
	{
		bool ret = true;
		for (const int i : RangeIter(4))
		{
			for (const int j : RangeIter(4))
			{
				ret = ret && m1(i, j) == m2(i, j);
			}
		}
		return ret;
	}
	inline bool operator!=(const Matrix& m1, const Matrix& m2)
	{
		return !(m1 == m2);
	}

	inline constexpr float VecElem(const Vec3& v, int index)
	{
		switch (index)
		{
			case 0: return v.x;
			case 1: return v.y;
		}
		return v.z;
	}

	template <std::totally_ordered T>
	inline constexpr T Clamp(T value, T min, T max)
	{
		if (value < min)
		{
			return min;
		}
		if (value > max)
		{
			return max;
		}
		return value;
	}

	inline constexpr float Dot(const Vec2& a, const Vec2& b)
	{
		return (a.x * b.x) + (a.y * b.y);
	}
	
	inline constexpr float Dot(const Vec3& a, const Vec3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	inline constexpr Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	inline constexpr Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	inline constexpr bool operator==(const Vec2& lhs, const Vec2& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	inline constexpr bool operator!=(const Vec2& lhs, const Vec2& rhs)
	{
		return !(lhs==rhs);
	}

	inline constexpr bool operator==(const Vec3& lhs, const Vec3& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	inline constexpr bool operator!=(const Vec3& lhs, const Vec3& rhs)
	{
		return !(lhs==rhs);
	}

	inline constexpr bool operator==(const Vec4& lhs, const Vec4& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
	}

	inline constexpr bool operator!=(const Vec4& lhs, const Vec4& rhs)
	{
		return !(lhs==rhs);
	}

	inline constexpr float SmoothStep(float v)
	{
		// x = 3(v^2) - 2(v^3)
		return v*v*(3-(2*v));
	}
	
	inline constexpr float SmootherStep(float t)
	{
		// From Perlin's improved noise function
		return t * t * t * (t * (t * 6 - 15) + 10);			// 6t^5 - 15t^4 + 10t^3
	}

	inline constexpr float Lerp(float min, float max, float lerp)
	{
		const float range = max-min;
		return min + lerp*range;
	}

	inline constexpr float SmoothLerp(float min, float max, float lerp)
	{
		return Lerp(min, max, SmoothStep(lerp));
	}

	inline constexpr float SmootherLerp(float min, float max, float lerp)
	{
		return Lerp(min, max, SmootherStep(lerp));
	}


	inline Vec3 normalise(const Vec3& v)
	{
		const XMVECTOR xv = XMLoadFloat3(&v);
		const XMVECTOR normalised = XMVector3Normalize(xv);
		Vec3 out;
		XMStoreFloat3(&out, normalised);
		return out;
	}

	inline Vec3 PositionFromMatrix(const Matrix& matrix)
	{
		//XMVECTOR scale;
		//XMVECTOR rotationQuat;
		//XMVECTOR translation;
		//XMMATRIX mat = XMLoadFloat4x4(&matrix);
		//bool success = XMMatrixDecompose(&scale, &rotationQuat, &translation, mat);
		//if (!success)
		//{
		//	return V30();
		//}
		//Vec3 position;
		//XMStoreFloat3(&position, translation);
		//return position;

		const auto& row = matrix.m[3];
		BE_ASSERT(row[3] != 0.f);
		const float invScale = 1.f / row[3];
		return Vec3(row[0] * invScale, row[1] * invScale, row[2] * invScale);
	}

	template <std::totally_ordered T>
	constexpr T Min(const T lhs, const T rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}
	
	template <std::totally_ordered T>
	constexpr T Max(const T lhs, const T rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}

	inline constexpr u32 rotl32(u32 x, u32 n)
	{
		return (x << n) | (x >> (32 - n));
	}

	constexpr int NextPowerOfTwo(int v)
	{
		// Fill all the bits from the top one, then add one. Subtract one at the start in case we're already a power of 2.
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}
};

