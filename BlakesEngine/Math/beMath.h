#pragma once
#include "external/xnamath/XNAMath.h"

#define PI 3.14159265358979f

namespace
{
	typedef XMFLOAT3 Vec3;

	float Length(const Vec3& v)
	{
		XMVECTOR xv = XMLoadFloat3(&v);
		return XMVector3Length(xv).m128_f32[0];
	}

	Vec3 To2DVec(const Vec3& v)
	{
		return Vec3(v.x, 0.0f, v.z);
	}

	Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
	{
		XMVECTOR l = XMLoadFloat3(&lhs);
		XMVECTOR r = XMLoadFloat3(&rhs);
		XMVECTOR xv = l-r;
		Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
	{
		XMVECTOR l = XMLoadFloat3(&lhs);
		XMVECTOR r = XMLoadFloat3(&rhs);
		XMVECTOR xv = l+r;
		Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	Vec3 operator*(const Vec3& v, const float& scale)
	{
		XMVECTOR l = XMLoadFloat3(&v);
		XMVECTOR xv = XMVectorScale(l, scale);
		Vec3 out;
		XMStoreFloat3(&out, xv);
		return out;
	}

	bool operator==(const Vec3& lhs, const Vec3& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	bool operator!=(const Vec3& lhs, const Vec3& rhs)
	{
		return !(lhs==rhs);
	}

	Vec3 normalise(const Vec3& v)
	{
		XMVECTOR xv = XMLoadFloat3(&v);
		XMVECTOR normalised = XMVector3Normalize(xv);
		Vec3 out;
		XMStoreFloat3(&out, normalised);
		return out;
	}
}

