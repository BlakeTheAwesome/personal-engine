#include "BlakesEngine/bePCH.h"
#include "beCameraUtils.h"
#include "beCamera.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Input/beMouse.h"

import beDirectXMath;
using namespace DirectXMath;

bool intersectsBounds(const Vec3& _boundsMin, const Vec3& _boundsMax, const XMVECTOR& rayStart, const XMVECTOR& rayDir, float* distance)
{
	// transform into local space
	const XMVECTOR boundsMin = XMLoadFloat3(&_boundsMin);
	const XMVECTOR boundsMax = XMLoadFloat3(&_boundsMax);

	// Check if the start is inside the bounding box
	const bool inside = XMVector3Greater(rayStart, boundsMin) && XMVector3Less(rayStart, boundsMax);

	// Test against the planes of the box
	float tNear = -FLT_MAX, tFar = FLT_MAX;
	int i = 0;
	for (; i < 3; i++)
	{
		const float dir1 = XMVectorGetByIndex(rayDir, i);
		const float pos1 = XMVectorGetByIndex(rayStart, i);
		const float min1 = XMVectorGetByIndex(boundsMin, i);
		const float max1 = XMVectorGetByIndex(boundsMax, i);
		if (dir1 == (float)0.0f)
		{
			// If not moving along this axes, and we're not within the bounds on this axes, there is no intersection.
			if (pos1 < min1 || pos1 > max1)
			{
				return false;
			}
		}
		else
		{
			float t1 = (min1 - pos1) / dir1;
			float t2 = (max1 - pos1) / dir1;
			if (t1 > t2)
			{
				std::swap(t1, t2);
			}
			tNear = Max(t1, tNear);
			tFar = Min(t2, tFar);
			if (tNear > tFar || tFar < 0.0f)
				break;
		}
	}

	// Check if testing did not find a separating plane
	if (inside && tNear < 0.0f)
	{
		tNear = tFar;
	}
	if (distance)
	{
		*distance = tNear;
	}
	return true;
}

std::optional<Vec3> beCameraUtils::WorldPosFromScreenPos(const beRenderInterface& ri, const Matrix& cameraViewMatrix, int x, int y)
{
	// performs a line-plane intersection from the camera position,coords of the click and field
	// to give is the world co-ords on the field the user clicked

	Vec3 worldPos, worldDir;
	if (!GetScreeenToWorldRay(ri, cameraViewMatrix, (float)x, (float)y, &worldPos, &worldDir))
	{
		return {};
	}


	float intersectionDist = 0.f;
	// Todo: Allow checks against other volumes

	const Vec3 minGroundBounds{-1000.f, -1000.f, -0.5f};
	const Vec3 maxGroundBounds{ 1000.f,  1000.f,  0.0f};

	const XMVECTOR worldPosV = XMLoadFloat3(&worldPos);
	const XMVECTOR worldDirV = XMLoadFloat3(&worldDir);
	const bool intersects = intersectsBounds(minGroundBounds, maxGroundBounds, worldPosV, worldDirV, &intersectionDist);
	if (!intersects) { return {}; }

	Vec3 result;
	XMStoreFloat3(&result, XMVectorMultiplyAdd(worldDirV, XMVectorReplicate(intersectionDist), worldPosV));
	return result;
}

bool beCameraUtils::GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& _viewMatrix, float screenX, float screenY, Vec3* pos, Vec3* direction)
{
	auto [screenWidth, screenHeight] = ri.GetScreenSize();
	if (screenX < 0
	 || screenY < 0
	 || screenX > screenWidth
	 || screenY > screenHeight)
	{
		*pos = V30();
		*direction = V30();
		return false;
	}
	
	beArray<Vec3, 2> inputVecs;
	beArray<Vec3, 2> outputVecs;

	inputVecs.at(0) = Vec3(screenX, screenY, 0.f);
	inputVecs.at(1) = Vec3(screenX, screenY, 1.f);

	const XMMATRIX viewMatrix = XMLoadFloat4x4(&_viewMatrix);
	const XMMATRIX projectionMatrix = XMLoadFloat4x4(&ri.GetProjectionMatrix());
	const XMMATRIX worldMatrix = XMLoadFloat4x4(&ri.GetWorldMatrix());

	XMVector3UnprojectStream(outputVecs.data(), sizeof(Vec3), inputVecs.data(), sizeof(Vec3), 2, 0.f, 0.f, screenWidth, screenHeight, 0.f, 1.f, projectionMatrix, viewMatrix, worldMatrix);

	const XMVECTOR worldPos    = XMLoadFloat3(&outputVecs.at(0));
	const XMVECTOR worldAlongZ = XMLoadFloat3(&outputVecs.at(1));
	const XMVECTOR worldForward = XMVector3Normalize(XMVectorSubtract(worldAlongZ, worldPos));


	XMStoreFloat3(pos, worldPos);
	XMStoreFloat3(direction, worldForward);

	return true;
}

std::optional<beCameraUtils::PosDir> beCameraUtils::GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& viewMatrix, const beMouse& mouse)
{
	Vec3 pos, direction;
	if (GetScreeenToWorldRay(ri, viewMatrix, (float)mouse.GetX(), (float)mouse.GetY(), &pos, &direction))
	{
		return PosDir{pos, direction};
	}
	return {};
}
