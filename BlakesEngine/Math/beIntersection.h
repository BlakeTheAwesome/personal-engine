#pragma once
#include "beMath.h"
#include <optional>
#include <tuple>

namespace beIntersection
{
	// Collisions will be 0 at rayStart, 1 at rayStart+rayDir, negative if behind, >1 if past rayStart+rayDir
	int TRayVsAABB(const Vec3& rayStart, const Vec3& rayDir, const Vec3& boxMin, const Vec3& boxMax, std::span<float, 2> collisions);
	std::optional<std::pair<float, float>> TRayVsAABB(const Vec3& rayStart, const Vec3& rayDir, const Vec3& boxMin, const Vec3& boxMax);

	// returns first collision from rayStart in rayDir
	std::optional<Vec3> RayVsAABB(const Vec3& rayStart, const Vec3& rayDir, const Vec3& boxMin, const Vec3& boxMax);
}
