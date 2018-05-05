#pragma once
#include "BlakesEngine/Math/beMath.h"

class beRenderInterface;

namespace beCameraUtils
{
	std::optional<Vec3> WorldPosFromScreenPos(const beRenderInterface& ri, const Matrix& cameraViewMatrix, const Vec3& cameraPos, int x, int y);
	bool GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& viewMatrix, float screenX, float screenY, float screenWidth, float screenHeight, Vec3* pos, Vec3* direction);
};
