#pragma once
#include <DirectXMath.h>
#include "BlakesEngine/Core/bePrintf.h"
#define PI XM_PI
#define RAD_TO_DEG(r) (r * (180.0f / PI))
#define DEG_TO_RAD(d) (d * (PI / 180.0f))

using namespace DirectX;

namespace beMath
{
	typedef XMFLOAT2 Vec2;
	typedef XMFLOAT3 Vec3;
	typedef XMFLOAT4 Vec4;
	typedef XMFLOAT4X4 Matrix;
	typedef XMFLOAT4 Quat;

	static inline Vec3 V30()  { return Vec3( 0.f,  0.f,  0.f); }
	static inline Vec3 V3X()  { return Vec3( 1.f,  0.f,  0.f); }
	static inline Vec3 V3Y()  { return Vec3( 0.f,  1.f,  0.f); }
	static inline Vec3 V3Z()  { return Vec3( 0.f,  0.f,  1.f); }
	static inline Vec3 V3NX() { return Vec3(-1.f,  0.f,  0.f); }
	static inline Vec3 V3NY() { return Vec3( 0.f, -1.f,  0.f); }
	static inline Vec3 V3NZ() { return Vec3( 0.f,  0.f, -1.f); }

	static inline Vec4 V40()  { return Vec4( 0.f,  0.f,  0.f,  0.f); }
	static inline Vec4 V4X()  { return Vec4( 1.f,  0.f,  0.f,  0.f); }
	static inline Vec4 V4Y()  { return Vec4( 0.f,  1.f,  0.f,  0.f); }
	static inline Vec4 V4Z()  { return Vec4( 0.f,  0.f,  1.f,  0.f); }
	static inline Vec4 V4W()  { return Vec4( 0.f,  0.f,  0.f,  1.f); }
	static inline Vec4 V4NX() { return Vec4(-1.f,  0.f,  0.f,  0.f); }
	static inline Vec4 V4NY() { return Vec4( 0.f, -1.f,  0.f,  0.f); }
	static inline Vec4 V4NZ() { return Vec4( 0.f,  0.f, -1.f,  0.f); }

	static inline Vec4 V4XW()  { return Vec4( 1.f,  0.f,  0.f,  1.f); }
	static inline Vec4 V4YW()  { return Vec4( 0.f,  1.f,  0.f,  1.f); }
	static inline Vec4 V4ZW()  { return Vec4( 0.f,  0.f,  1.f,  1.f); }
	static inline Vec4 V4NXW() { return Vec4(-1.f,  0.f,  0.f,  1.f); }
	static inline Vec4 V4NYW() { return Vec4( 0.f, -1.f,  0.f,  1.f); }
	static inline Vec4 V4NZW() { return Vec4( 0.f,  0.f, -1.f,  1.f); }

	static inline Vec4 V4XY() { return Vec4(1.f, 1.f, 0.f, 0.f); }
	static inline Vec4 V4XZ() { return Vec4(1.f, 0.f, 1.f, 0.f); }
	static inline Vec4 V4YZ() { return Vec4(0.f, 1.f, 1.f, 0.f); }
	static inline Vec4 V4XYW() { return Vec4(1.f, 1.f, 0.f, 1.f); }
	static inline Vec4 V4XZW() { return Vec4(1.f, 0.f, 1.f, 1.f); }
	static inline Vec4 V4YZW() { return Vec4(0.f, 1.f, 1.f, 1.f); }

	inline float Length(const beMath::Vec3& v)
	{
		XMVECTOR xv = XMLoadFloat3(&v);
		XMFLOAT4A out; XMStoreFloat4A(&out, xv);
		return out.x;
	}

	inline beMath::Vec3 To2DVec(const beMath::Vec3& v)
	{
		return beMath::Vec3(v.x, 0.0f, v.z);
	}

	inline beMath::Vec3 operator-(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		XMVECTOR l = XMLoadFloat3(&lhs);
		XMVECTOR r = XMLoadFloat3(&rhs);
		XMVECTOR xv = XMVectorSubtract(l, r);
		beMath::Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline beMath::Vec3 operator+(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		XMVECTOR l = XMLoadFloat3(&lhs);
		XMVECTOR r = XMLoadFloat3(&rhs);
		XMVECTOR xv = XMVectorAdd(l, r);
		beMath::Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline beMath::Vec3 operator*(const beMath::Vec3& v, const float& scale)
	{
		XMVECTOR l = XMLoadFloat3(&v);
		XMVECTOR xv = XMVectorScale(l, scale);
		beMath::Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	inline beMath::Vec4 operator-(const beMath::Vec4& lhs, const beMath::Vec4& rhs)
	{
		XMVECTOR l = XMLoadFloat4(&lhs);
		XMVECTOR r = XMLoadFloat4(&rhs);
		XMVECTOR xv = XMVectorSubtract(l, r);
		beMath::Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	inline beMath::Vec4 operator+(const beMath::Vec4& lhs, const beMath::Vec4& rhs)
	{
		XMVECTOR l = XMLoadFloat4(&lhs);
		XMVECTOR r = XMLoadFloat4(&rhs);
		XMVECTOR xv = XMVectorAdd(l, r);
		beMath::Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	inline beMath::Vec4 operator*(const beMath::Vec4& v, const float& scale)
	{
		XMVECTOR l = XMLoadFloat4(&v);
		XMVECTOR xv = XMVectorScale(l, scale);
		beMath::Vec4 out;
		XMStoreFloat4(&out, xv);
		return out;
	}

	template <typename T>
	inline T Clamp(T value, T min, T max)
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

	inline float Dot(const beMath::Vec2& a, const beMath::Vec2& b)
	{
		return (a.x * b.x) + (a.y * b.y);
	}
	
	inline float Dot(const beMath::Vec3& a, const beMath::Vec3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	inline bool operator==(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	inline bool operator!=(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
	{
		return !(lhs==rhs);
	}

	inline float SmoothStep(float v)
	{
		// x = 3(v^2) - 2(v^3)
		return v*v*(3-(2*v));
	}
	
	inline float SmootherStep(float t)
	{
		// From Perlin's improved noise function
		return t * t * t * (t * (t * 6 - 15) + 10);			// 6t^5 - 15t^4 + 10t^3
	}

	inline float Lerp(float min, float max, float lerp)
	{
		float range = max-min;
		return min + lerp*range;
	}

	inline float SmoothLerp(float min, float max, float lerp)
	{
		return Lerp(min, max, SmoothStep(lerp));
	}

	inline float SmootherLerp(float min, float max, float lerp)
	{
		return Lerp(min, max, SmootherStep(lerp));
	}


	inline beMath::Vec3 normalise(const beMath::Vec3& v)
	{
		XMVECTOR xv = XMLoadFloat3(&v);
		XMVECTOR normalised = XMVector3Normalize(xv);
		beMath::Vec3 out;
		XMStoreFloat3(&out, normalised);
		return out;
	}

	inline beMath::Vec3 PositionFromMatrix(const beMath::Matrix& matrix)
	{
		XMVECTOR scale;
		XMVECTOR rotationQuat;
		XMVECTOR translation;
		XMMATRIX mat = XMLoadFloat4x4(&matrix);
		bool success = XMMatrixDecompose(&scale, &rotationQuat, &translation, mat);
		if (!success)
		{
			return beMath::Vec3(0.f, 0.f, 0.f);
		}
		Vec3 position;
		XMStoreFloat3(&position, translation);

		//bePRINTF("CameraPosition %f, %f, %f, %f", translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2], translation.m128_f32[3]);
		//bePRINTF("CameraScale %f, %f, %f, %f", scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2], scale.m128_f32[3]);
		//bePRINTF("CameraRotation %f, %f, %f, %f", rotationQuat.m128_f32[0], rotationQuat.m128_f32[1], rotationQuat.m128_f32[2], rotationQuat.m128_f32[3]);

		//XMVECTOR pos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		//XMVECTOR newPos = XMVector3Transform(pos, mat);

		//bePRINTF("neaPos %f, %f, %f, %f", newPos.m128_f32[0], newPos.m128_f32[1], newPos.m128_f32[2], newPos.m128_f32[3]);

		return position;
	}

	template <typename T>
	T Min(const T lhs, const T rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}
	
	template <typename T>
	T Max(const T lhs, const T rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}

	template <typename T>
	static inline T rotl32(T x, const int n)
	{
		return (x << n) | (x >> (32 - n));
	}
	template <typename T>
	static inline T rotr32(T x, const int n)
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

