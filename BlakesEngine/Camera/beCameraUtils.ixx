module;
#include <optional>

export module beCameraUtils;

import beRenderInterface;
import beMath;
import beMouse;

export namespace beCameraUtils
{
	struct PosDir
	{
		Vec3 pos;
		Vec3 dir;
	};

	std::optional<Vec3> WorldPosFromScreenPos(const beRenderInterface& ri, const Matrix& cameraViewMatrix, int x, int y);
	bool GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& viewMatrix, float screenX, float screenY, Vec3* pos, Vec3* direction);
	std::optional<PosDir> GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& viewMatrix, const beMouse& mouse);
};
