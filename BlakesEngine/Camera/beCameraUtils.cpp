#include "BlakesEngine/bePCH.h"
#include "beCameraUtils.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"

bool beCameraUtils::GetScreeenToWorldRay(const beRenderInterface& ri, const Matrix& _viewMatrix, float screenX, float screenY, float screenWidth, float screenHeight, Vec3* pos, Vec3* direction)
{
	if (screenX < 0
	 || screenY < 0
	 || screenX > screenWidth
	 || screenY > screenHeight)
	{
		*pos = Vec3(0.f, 0.f, 0.f);
		*direction = Vec3(0.f, 0.f, 0.f);
		return false;
	}
	
	XMMATRIX projectionMatrix = XMLoadFloat4x4(&ri.GetProjectionMatrix());
	XMMATRIX viewMatrix = XMLoadFloat4x4(&_viewMatrix);
	XMMATRIX viewProjection = viewMatrix * projectionMatrix;
	XMVECTOR viewDeterminant = XMMatrixDeterminant(viewProjection);
	XMMATRIX inverseProjectionView = XMMatrixInverse(&viewDeterminant, viewProjection);

	float x = ((2.f * (screenX / screenWidth)) - 1.f);
	float y = -((2.f * (screenY / screenHeight)) - 1.f);

	bePRINTF("relativeXY: %.2f,%.2f", x, y);

	XMVECTOR screenPosVec = XMVectorSet(x, y, 0.f, 0.f);
	XMVECTOR screenForwardVec = XMVectorSet(x, y, 1.f, 0.f);

	XMVECTOR worldPos = XMVector3Transform(screenPosVec, inverseProjectionView);
	XMVECTOR worldAlongZ = XMVector3Transform(screenForwardVec, inverseProjectionView);
	XMVECTOR worldForward = XMVector3Normalize(worldAlongZ - worldPos);

	XMStoreFloat3(pos, worldPos);
	XMStoreFloat3(pos, worldForward);

	return true;
}
