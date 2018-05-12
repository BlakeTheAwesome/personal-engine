#include "BlakesEngine/bePCH.h"
#include "beFlightCamera.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/beMouse.h"

static const float ROTATIONS_PER_SECOND = 0.5f * (2.f * PI);
static const float DISTANCE_PER_SECOND = 1.0f;
static const float MOUSE_SPEED_MULTIPLIER = 0.01f;
static const float R2_MULTIPLIER = 10.f;
static const bool INVERT_Y = false;
static const bool INVERT_X = false;



//Could even go for three basic quads with a different colour shader on each.


beFlightCamera::beFlightCamera()
	{
	/*XMVECTOR startingPosition = XMLoadFloat3(&m_position);
	XMVECTOR startingLookat = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	XMVECTOR startingUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMMATRIX startingMatrix = XMMatrixLookAtLH(startingPosition, startingLookat, startingUp);
	XMStoreFloat4x4(&m_matrix, startingMatrix);*/

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(m_yaw, m_pitch, m_roll);
	XMMATRIX startingOrientation = XMMatrixRotationQuaternion(quat);
	XMStoreFloat4x4(&m_orientationMatrix, startingOrientation);

	UpdateImpl(0.f, 0.f, 0.f, 0.f, 0.f);

	//XMMATRIX startingOrientation = XMMatrixLookToLH(XMVectorSet(0.f, 0.f, 0.f, 0.f), -XMLoadFloat3(&m_position), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//XMStoreFloat4x4(&m_orientationMatrix, startingOrientation);
}

beFlightCamera::~beFlightCamera()
{
	BE_ASSERT(!m_gamepad);
}

void beFlightCamera::AttachGamepad(beGamepad* gamepad)
{
	BE_ASSERT(!m_gamepad);
	m_gamepad = gamepad;
}

void beFlightCamera::DetachGamepad()
{
	BE_ASSERT(m_gamepad);
	m_gamepad = nullptr;
}

void beFlightCamera::AttachMouse(beMouse* mouse)
{
	BE_ASSERT(!m_mouse);
	m_mouse = mouse;
}

void beFlightCamera::DetachMouse()
{
	BE_ASSERT(m_mouse);
	m_mouse = nullptr;
}

void beFlightCamera::Update(float dt)
{
	if (m_gamepad)
	{
		float lX = m_gamepad->GetLeftX();
		float lY = m_gamepad->GetLeftY();
		float rX = m_gamepad->GetRightX();
		float rY = m_gamepad->GetRightY();

		if (lX != 0.f || lY != 0.f || rX != 0.f || rY != 0.f)
		{
			float moveSpeedFactor = (5.f + (R2_MULTIPLIER*m_gamepad->GetR2()));

			float extraPitch = (INVERT_Y ? rY : -rY) * ROTATIONS_PER_SECOND * dt;
			float extraYaw = (INVERT_X ? -rX : rX) * ROTATIONS_PER_SECOND * dt;
			float forwards = -lY * DISTANCE_PER_SECOND * dt * moveSpeedFactor;
			float right = (INVERT_X ? -lX : lX) * DISTANCE_PER_SECOND * dt * moveSpeedFactor;
			UpdateImpl(dt, extraPitch, extraYaw, forwards, right);
		}
	}

	if (m_mouse)
	{
		bool lDown = m_mouse->IsDown(beMouse::Button::LeftButton);
		bool rDown = m_mouse->IsDown(beMouse::Button::RightButton);
		float extraPitch = lDown ? -m_mouse->GetYMovement() * MOUSE_SPEED_MULTIPLIER : 0.f;
		float extraYaw = lDown ? -m_mouse->GetXMovement() * MOUSE_SPEED_MULTIPLIER : 0.f;
		float forwards = rDown ? m_mouse->GetYMovement() * MOUSE_SPEED_MULTIPLIER : 0.f;
		float right = rDown ? m_mouse->GetXMovement() * MOUSE_SPEED_MULTIPLIER : 0.f;
		UpdateImpl(dt, extraPitch, extraYaw, forwards, right);
	}
}

void beFlightCamera::UpdateImpl(float dt, float extraPitch, float extraYaw, float extraForwards, float extraRight)
{
	/*XMMATRIX initialMatrix = XMLoadFloat4x4(&m_matrix);
	XMMATRIX rotX = XMMatrixRotationX(extraPitch);
	XMMATRIX rotY = XMMatrixRotationY(extraYaw);
	XMMATRIX translation = XMMatrixTranslation(forwards, 0.f, right);
	XMMATRIX newMatrix = initialMatrix * rotX * rotY * translation;

	XMStoreFloat4x4(&m_matrix, newMatrix);*/



	// Adapted from GameDev forum

	//member floats that accrue the current Yaw,Pitch and Roll

	m_pitch += extraPitch;
	m_pitch = beMath::Clamp(m_pitch, (float)-M_PI, 0.f);
	m_yaw += extraYaw;
	m_roll += 0.f;

	//XMVECTOR quat = XMQuaternionRotationRollPitchYaw(m_pitch, m_yaw, m_roll);
	//XMVECTOR quat2 = XMQuaternionRotationRollPitchYaw(m_pitch, m_roll, m_yaw);
	//
	//XMMATRIX orientation = XMMatrixRotationQuaternion(quat);
	//orientation = XMMatrixInverse(nullptr, orientation);

	//XMVECTOR res1, res2;
	//float res1f, res2f;
	//XMQuaternionToAxisAngle(&res1, &res1f, quat);
	//XMQuaternionToAxisAngle(&res2, &res2f, quat2);
	//
	//LOG("pitch:{:2f} yaw:{:2f} roll:{:2f}\n", m_pitch, m_yaw, m_roll);
	//LOG("quat1:{:2f},{:2f},{:2f},{:2f}\n", XMVectorGetByIndex(quat, 0), XMVectorGetByIndex(quat, 1), XMVectorGetByIndex(quat, 2), XMVectorGetByIndex(quat, 3));
	//LOG("quat2:{:2f},{:2f},{:2f},{:2f}\n", XMVectorGetByIndex(quat2, 0), XMVectorGetByIndex(quat2, 1), XMVectorGetByIndex(quat2, 2), XMVectorGetByIndex(quat2, 3));
	//LOG("res1:{:2f},{:2f},{:2f},{:2f}   - {:2f}\n", XMVectorGetByIndex(res1, 0), XMVectorGetByIndex(res1, 1), XMVectorGetByIndex(res1, 2), XMVectorGetByIndex(res1, 3), res1f);
	//LOG("res2:{:2f},{:2f},{:2f},{:2f}   - {:2f}\n", XMVectorGetByIndex(res2, 0), XMVectorGetByIndex(res2, 1), XMVectorGetByIndex(res2, 2), XMVectorGetByIndex(res2, 3), res2f);
	//member floats that temporarily contain the current



	//have to rotate around z(up) then around right(x)
	float yaw = m_yaw;
	//if (sin(m_pitch) > 0)
	//{
	//	yaw = -yaw;
	//}

	XMMATRIX rotZ = XMMatrixRotationZ(yaw);
	XMMATRIX rotX = XMMatrixRotationX(m_pitch);
	XMMATRIX orientation = rotZ * rotX;
	orientation = XMMatrixInverse(nullptr, orientation);


	//XMVECTOR angles = XMVectorSet(m_pitch, m_yaw, m_roll, 0.0f);
	//XMVECTOR quat;
	//{
	//	static const XMVECTORF32  Sign ={{{1.0f, -1.0f, -1.0f, 1.0f}}};
	//
	//	XMVECTOR HalfAngles = XMVectorMultiply(angles, g_XMOneHalf.v);
	//
	//	XMVECTOR SinAngles, CosAngles;
	//	XMVectorSinCos(&SinAngles, &CosAngles, HalfAngles);
	//
	//	XMVECTOR P0 = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1X, XM_PERMUTE_1X>(SinAngles, CosAngles);
	//	XMVECTOR Y0 = XMVectorPermute<XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1Y>(SinAngles, CosAngles);
	//	XMVECTOR R0 = XMVectorPermute<XM_PERMUTE_1Z, XM_PERMUTE_1Z, XM_PERMUTE_0Z, XM_PERMUTE_1Z>(SinAngles, CosAngles);
	//	XMVECTOR P1 = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1X, XM_PERMUTE_1X>(CosAngles, SinAngles);
	//	XMVECTOR Y1 = XMVectorPermute<XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1Y>(CosAngles, SinAngles);
	//	XMVECTOR R1 = XMVectorPermute<XM_PERMUTE_1Z, XM_PERMUTE_1Z, XM_PERMUTE_0Z, XM_PERMUTE_1Z>(CosAngles, SinAngles);
	//
	//	XMVECTOR Q1 = XMVectorMultiply(P1, Sign.v);
	//	XMVECTOR Q0 = XMVectorMultiply(P0, Y0);
	//	Q1 = XMVectorMultiply(Q1, Y1);
	//	Q0 = XMVectorMultiply(Q0, R0);
	//	quat = XMVectorMultiplyAdd(Q1, R1, Q0);
	//}
	//
	//XMMATRIX orientation = XMMatrixRotationQuaternion(quat);
	//orientation = XMMatrixInverse(nullptr, orientation);
















	//XMMATRIX orientation = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll);
	//XMVECTOR scale, quat, origin;
	//XMMatrixDecompose(&scale, &quat, &origin, orientation);
	//quat = XMVectorSwizzle(quat, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_X);


	//cycles input
	XMVECTOR currentPosition = XMVectorSet(m_position.x, m_position.y, m_position.z, 1.f);
	XMVECTOR translation = XMVectorSet(extraRight, 0.f, extraForwards, 0.f);
	XMVECTOR transformedTranslation = XMVector3TransformNormal(translation, orientation);
	currentPosition += transformedTranslation;


	//this is a hack to keep a constant level from the ground

	//remove this line for a thrusting spaceship type movement

	//m_vecPosition.y = m_YPos;


			//fill the quaternion with your rotation info

	//combine your new position offset with your rotation quaternion

	//this yields an orientation matrix

	//XMVECTOR scale = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	//XMVECTOR origin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	//XMMATRIX newMat = XMMatrixAffineTransformation(scale, origin, quat, currentPosition);


	XMMATRIX newMat = orientation;
	newMat.r[3] = XMVectorAdd(newMat.r[3], currentPosition);
	


	//the orientation matrix describes a view that stares directly at

	//the camera, invert it to get your view matrix.

	XMMATRIX viewMatrix = XMMatrixInverse(nullptr, newMat);

	XMStoreFloat4x4(&m_orientationMatrix, orientation);
	XMStoreFloat4x4(&m_viewMatrix, viewMatrix);
	XMStoreFloat3(&m_position, currentPosition);

}

