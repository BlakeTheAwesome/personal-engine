module;

#include "BlakesEngine/bePCH.h"
#include <cmath>

module beIntersection;

import RangeIter;

int beIntersection::TRayVsAABB(const Vec3& rayStart, const Vec3& rayDir, const Vec3& boxMin, const Vec3& boxMax, std::span<float, 2> collisions)
{
	float maxS = -FLT_MAX;
	float minT = FLT_MAX;

	for (const int i : RangeIter(3))
	{
		const float dir1 = VecElem(rayDir, i);
		const float start1 = VecElem(rayStart, i);
		const float boxMin1 = VecElem(boxMin, i);
		const float boxMax1 = VecElem(boxMax, i);

		if (std::fabs(dir1) < 0.0001f)
		{
			if (start1 < boxMin1 || start1 > boxMax1)
			{
				return 0;
			}
		}
		else
		{
			const float invDir = 1.f/dir1;
			float s = (boxMin1 - start1) * invDir;
			float t = (boxMax1 - start1) * invDir;
			if (s > t)
			{
				std::swap(s, t);
			}
			maxS = std::max(maxS, s);
			minT = std::min(minT, t);

			if (minT < 0.f || maxS > minT)
			{
				return 0;
			}
		}
	}

	collisions[0] = maxS;
	collisions[1] = minT;
	return 2;
}

std::optional<std::pair<float, float>> beIntersection::TRayVsAABB(const Vec3& rayStart, const Vec3& rayDir, const Vec3& boxMin, const Vec3& boxMax)
{
	float collisions[2];
	if (TRayVsAABB(rayStart, rayDir, boxMin, boxMax, collisions) > 0)
	{
		return std::pair<float, float>{collisions[0], collisions[1]};
	}
	return {};

}


std::optional<Vec3> beIntersection::RayVsAABB(const Vec3& rayStart, const Vec3& rayDir, const Vec3& boxMin, const Vec3& boxMax)
{
	if (auto collisions = TRayVsAABB(rayStart, rayDir, boxMin, boxMax))
	{
		return rayStart + (rayDir * collisions->first);
	}
	return {};
}
