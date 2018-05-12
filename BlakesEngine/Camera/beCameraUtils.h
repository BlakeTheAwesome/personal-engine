#pragma once
#include "BlakesEngine/Math/beMath.h"

class beRenderInterface;
class beMouse;

namespace beCameraUtils
{
	struct PosDir
	{
		Vec3 pos;
		Vec3 dir;
	};

	std::optional<Vec3> WorldPosFromScreenPos(const beRenderInterface& ri, const Matrix& cameraViewMatrix, const Vec3& cameraPos, int x, int y);
	bool GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& viewMatrix, float screenX, float screenY, float screenWidth, float screenHeight, Vec3* pos, Vec3* direction);
	std::optional<PosDir> GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& viewMatrix, const beMouse& mouse);
};
