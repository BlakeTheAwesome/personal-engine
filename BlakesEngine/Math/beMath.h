#pragma once
#include <DirectXMath.h>

#define PI 3.14159265358979f
#define RAD_TO_DEG(r) (r * (180.0f / PI))
#define DEG_TO_RAD(d) (d * (PI / 180.0f))

using namespace DirectX;

namespace beMath
{
	typedef XMFLOAT3 Vec3;
	typedef XMFLOAT4 Vec4;
	typedef XMFLOAT4X4 Matrix;
};


float Length(const beMath::Vec3& v)
{
	XMVECTOR xv = XMLoadFloat3(&v);
	return XMVector3Length(xv).m128_f32[0];
}

beMath::Vec3 To2DVec(const beMath::Vec3& v)
{
	return beMath::Vec3(v.x, 0.0f, v.z);
}

beMath::Vec3 operator-(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
{
	XMVECTOR l = XMLoadFloat3(&lhs);
	XMVECTOR r = XMLoadFloat3(&rhs);
	XMVECTOR xv = l-r;
	beMath::Vec3 out;
	XMStoreFloat3(&out, xv);
	return out;
}

beMath::Vec3 operator+(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
{
	XMVECTOR l = XMLoadFloat3(&lhs);
	XMVECTOR r = XMLoadFloat3(&rhs);
	XMVECTOR xv = l+r;
	beMath::Vec3 out;
	XMStoreFloat3(&out, xv);
	return out;
}

beMath::Vec3 operator*(const beMath::Vec3& v, const float& scale)
{
	XMVECTOR l = XMLoadFloat3(&v);
	XMVECTOR xv = XMVectorScale(l, scale);
	beMath::Vec3 out;
	XMStoreFloat3(&out, xv);
	return out;
}

bool operator==(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

bool operator!=(const beMath::Vec3& lhs, const beMath::Vec3& rhs)
{
	return !(lhs==rhs);
}

beMath::Vec3 normalise(const beMath::Vec3& v)
{
	XMVECTOR xv = XMLoadFloat3(&v);
	XMVECTOR normalised = XMVector3Normalize(xv);
	beMath::Vec3 out;
	XMStoreFloat3(&out, normalised);
	return out;
}

using namespace beMath;

