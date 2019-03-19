#pragma once
#include <DirectXMath.h>
#include "BlakesEngine/Core/bePrintf.h"
#define PI XM_PI
#define RAD_TO_DEG(r) (r * (180.0f / PI))
#define DEG_TO_RAD(d) (d * (PI / 180.0f))

using namespace DirectX;

namespace beMath
{
	using Vec2 = XMFLOAT2;
	using Vec3 = XMFLOAT3;
	using Vec4 = XMFLOAT4;
	using Matrix = XMFLOAT4X4;
	using Quat = XMFLOAT4;

	static inline Vec2 V20()  { return Vec2( 0.f,  0.f); }
	static inline Vec2 V21()  { return Vec2( 1.f,  1.f); }
	static inline Vec2 V2X()  { return Vec2( 1.f,  0.f); }
	static inline Vec2 V2Y()  { return Vec2( 0.f,  1.f); }
	static inline Vec2 V2N1() { return Vec2(-1.f, -1.f); }
	static inline Vec2 V2NX() { return Vec2(-1.f,  0.f); }
	static inline Vec2 V2NY() { return Vec2( 0.f, -1.f); }

	static inline Vec3 V30()  { return Vec3( 0.f,  0.f,  0.f); }
	static inline Vec3 V31()  { return Vec3( 1.f,  1.f,  1.f); }
	static inline Vec3 V3X()  { return Vec3( 1.f,  0.f,  0.f); }
	static inline Vec3 V3Y()  { return Vec3( 0.f,  1.f,  0.f); }
	static inline Vec3 V3Z()  { return Vec3( 0.f,  0.f,  1.f); }
	static inline Vec3 V3N1() { return Vec3(-1.f, -1.f, -1.f); }
	static inline Vec3 V3NX() { return Vec3(-1.f,  0.f,  0.f); }
	static inline Vec3 V3NY() { return Vec3( 0.f, -1.f,  0.f); }
	static inline Vec3 V3NZ() { return Vec3( 0.f,  0.f, -1.f); }

	static inline Vec4 V40()   { return Vec4( 0.f,  0.f,  0.f,  0.f); }
	static inline Vec4 V41()   { return Vec4( 1.f,  1.f,  1.f,  1.f); }
	static inline Vec4 V4X()   { return Vec4( 1.f,  0.f,  0.f,  0.f); }
	static inline Vec4 V4Y()   { return Vec4( 0.f,  1.f,  0.f,  0.f); }
	static inline Vec4 V4Z()   { return Vec4( 0.f,  0.f,  1.f,  0.f); }
	static inline Vec4 V4W()   { return Vec4( 0.f,  0.f,  0.f,  1.f); }
	static inline Vec4 V4N1()  { return Vec4(-1.f, -1.f, -1.f, -1.f); }
	static inline Vec4 V4NX()  { return Vec4(-1.f,  0.f,  0.f,  0.f); }
	static inline Vec4 V4NY()  { return Vec4( 0.f, -1.f,  0.f,  0.f); }
	static inline Vec4 V4NZ()  { return Vec4( 0.f,  0.f, -1.f,  0.f); }
	static inline Vec4 V4XW()  { return Vec4( 1.f,  0.f,  0.f,  1.f); }
	static inline Vec4 V4YW()  { return Vec4( 0.f,  1.f,  0.f,  1.f); }
	static inline Vec4 V4ZW()  { return Vec4( 0.f,  0.f,  1.f,  1.f); }
	static inline Vec4 V4NXW() { return Vec4(-1.f,  0.f,  0.f,  1.f); }
	static inline Vec4 V4NYW() { return Vec4( 0.f, -1.f,  0.f,  1.f); }
	static inline Vec4 V4NZW() { return Vec4( 0.f,  0.f, -1.f,  1.f); }
	static inline Vec4 V4XY()  { return Vec4( 1.f,  1.f,  0.f,  0.f); }
	static inline Vec4 V4XZ()  { return Vec4( 1.f,  0.f,  1.f,  0.f); }
	static inline Vec4 V4YZ()  { return Vec4( 0.f,  1.f,  1.f,  0.f); }
	static inline Vec4 V4XYW() { return Vec4( 1.f,  1.f,  0.f,  1.f); }
	static inline Vec4 V4XZW() { return Vec4( 1.f,  0.f,  1.f,  1.f); }
	static inline Vec4 V4YZW() { return Vec4( 0.f,  1.f,  1.f,  1.f); }

	inline float Length(const beMath::Vec3& v)
	{
		const XMVECTOR xv = XMLoadFloat3(&v);
		XMFLOAT4A out; XMStoreFloat4A(&out, xv);
		return out.x;
	}

	inline beMath::Vec4 ToVec4(const beMath::Vec3& v, float w)
	{
		return beMath::Vec4(v.x, v.y, v.z, w);
	}

	inline Matrix IdentityMatrix()
	{
		return Matrix(1.f, 0.f, 0.f, 0.f,
					  0.f, 1.f, 0.f, 0.f,
					  0.f, 0.f, 1.f, 0.f,
					  0.f, 0.f, 0.f, 1.f);
	}

	inline beMath::Vec2& operator*=(beMath::Vec2& lhs, const beMath::Vec2& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		return lhs;
	}

	inline beMath::Vec3& operator*=(beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		lhs.z *= rhs.z;
		return lhs;
	}

	inline beMath::Vec4& operator*=(beMath::Vec4& lhs, const beMath::Vec4& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		lhs.z *= rhs.z;
		lhs.w *= rhs.w;
		return lhs;
	}

	inline beMath::Vec2 operator*(const beMath::Vec2& lhs, const beMath::Vec2& rhs)
	{
		return {
			lhs.x * rhs.x,
			lhs.y * rhs.y,
		};
	}

	inline beMath::Vec3 operator*(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		return {
			lhs.x * rhs.x,
			lhs.y * rhs.y,
			lhs.z * rhs.z,
		};
	}

	inline beMath::Vec4 operator*(const beMath::Vec4& lhs, const beMath::Vec4& rhs)
	{
		return {
			lhs.x * rhs.x,
			lhs.y * rhs.y,
			lhs.z * rhs.z,
			lhs.w * rhs.w,
		};
	}

	inline beMath::Vec3 operator-(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		const XMVECTOR l = XMLoadFloat3(&lhs);
		const XMVECTOR r = XMLoadFloat3(&rhs);
		const XMVECTOR xv = XMVectorSubtract(l, r);
		beMath::Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline beMath::Vec3 operator+(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		const XMVECTOR l = XMLoadFloat3(&lhs);
		const XMVECTOR r = XMLoadFloat3(&rhs);
		const XMVECTOR xv = XMVectorAdd(l, r);
		beMath::Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline beMath::Vec3 operator*(const beMath::Vec3& v, const float& scale)
	{
		const XMVECTOR l = XMLoadFloat3(&v);
		const XMVECTOR xv = XMVectorScale(l, scale);
		beMath::Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline beMath::Vec4 operator-(const beMath::Vec4& lhs, const beMath::Vec4& rhs)
	{
		const XMVECTOR l = XMLoadFloat4(&lhs);
		const XMVECTOR r = XMLoadFloat4(&rhs);
		const XMVECTOR xv = XMVectorSubtract(l, r);
		beMath::Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	inline beMath::Vec4 operator+(const beMath::Vec4& lhs, const beMath::Vec4& rhs)
	{
		const XMVECTOR l = XMLoadFloat4(&lhs);
		const XMVECTOR r = XMLoadFloat4(&rhs);
		const XMVECTOR xv = XMVectorAdd(l, r);
		beMath::Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	inline beMath::Vec4 operator*(const beMath::Vec4& v, const float& scale)
	{
		const XMVECTOR l = XMLoadFloat4(&v);
		const XMVECTOR xv = XMVectorScale(l, scale);
		beMath::Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	inline bool operator==(const beMath::Matrix& m1, const beMath::Matrix& m2)
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
	inline bool operator!=(const beMath::Matrix& m1, const beMath::Matrix& m2)
	{
		return !(m1 == m2);
	}

	inline constexpr float VecElem(const beMath::Vec3& v, int index)
	{
		switch (index)
		{
			case 0: return v.x;
			case 1: return v.y;
		}
		return v.z;
	}

	template <typename T>
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

	inline constexpr float Dot(const beMath::Vec2& a, const beMath::Vec2& b)
	{
		return (a.x * b.x) + (a.y * b.y);
	}
	
	inline constexpr float Dot(const beMath::Vec3& a, const beMath::Vec3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	inline constexpr beMath::Vec2 operator+(const beMath::Vec2& lhs, const beMath::Vec2& rhs)
	{
		return beMath::Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	inline constexpr beMath::Vec2 operator-(const beMath::Vec2& lhs, const beMath::Vec2& rhs)
	{
		return beMath::Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	inline constexpr bool operator==(const beMath::Vec2& lhs, const beMath::Vec2& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	inline constexpr bool operator!=(const beMath::Vec2& lhs, const beMath::Vec2& rhs)
	{
		return !(lhs==rhs);
	}

	inline constexpr bool operator==(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	inline constexpr bool operator!=(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		return !(lhs==rhs);
	}

	inline constexpr bool operator==(const beMath::Vec4& lhs, const beMath::Vec4& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
	}

	inline constexpr bool operator!=(const beMath::Vec4& lhs, const beMath::Vec4& rhs)
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


	inline beMath::Vec3 normalise(const beMath::Vec3& v)
	{
		const XMVECTOR xv = XMLoadFloat3(&v);
		const XMVECTOR normalised = XMVector3Normalize(xv);
		beMath::Vec3 out;
		XMStoreFloat3(&out, normalised);
		return out;
	}

	inline beMath::Vec3 PositionFromMatrix(const beMath::Matrix& matrix)
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

	template <typename T>
	constexpr T Min(const T lhs, const T rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}
	
	template <typename T>
	constexpr T Max(const T lhs, const T rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}

	template <typename T>
	static inline constexpr T rotl32(T x, const int n)
	{
		return (x << n) | (x >> (32 - n));
	}
	template <typename T>
	static inline constexpr T rotr32(T x, const int n)
	{
		return (x << n) | (x >> (32 - n));
	}

	template <typename T>
	constexpr T tOR(T a, T b) { return (T)(a | b); }
	template <typename T>
	constexpr T tSUB(T a, T b) { return (T)(a - b); }
	template <typename T>
	constexpr T tADD(T a, T b) { return (T)(a + b); }
	template <typename T>
	constexpr T tMASK(T a, T mask) { return (T)(a & mask); }
	template <typename T>
	constexpr T tRShiftOR(T a, T shift) { return (T)(a | (a >> shift)); }
	template <typename T>
	constexpr T tLShiftORMask(T a, T shift, T mask) { return (T)((a | (a << shift)) & mask); }
	template <typename T>
	constexpr T tRShiftORMask(T a, T shift, T mask) { return (T)((a | (a >> shift)) & mask); }


	constexpr int NextPowerOfTwo(int v)
	{
		// Fill all the bits from the top one, then add one. Subtract one at the start in case we're already a power of 2.
		//v--;
		//v |= v >> 1;
		//v |= v >> 2;
		//v |= v >> 4;
		//v |= v >> 8;
		//v |= v >> 16;
		//v++;
		//return v;

		return tADD(tRShiftOR(tRShiftOR(tRShiftOR(tRShiftOR(tRShiftOR(tSUB(v, 1), 1), 2), 4), 8), 16), 1);
	}


};
using namespace beMath;

