#pragma once
#include "BlakesEngine/Math/beMath.h"

class beRenderInterface;

namespace beCameraUtils
{
	bool GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& viewMatrix, float screenX, float screenY, float screenWidth, float screenHeight, Vec3* pos, Vec3* direction);
};
