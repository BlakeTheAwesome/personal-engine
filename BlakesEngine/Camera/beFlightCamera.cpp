#include "BlakesEngine/bePCH.h"
#include "beFlightCamera.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/beMouse.h"

static constexpr float ROTATIONS_PER_SECOND = (float)(0.5 * (2.0 * M_2_PI));
static constexpr float DISTANCE_PER_SECOND = 1.0f;
static constexpr float MOUSE_SPEED_MULTIPLIER = 0.01f;
static constexpr float R2_MULTIPLIER = 10.f;
static constexpr bool INVERT_Y = false;
static constexpr bool INVERT_X = false;

beFlightCamera::beFlightCamera()
{
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(m_yaw, m_pitch, m_roll);
	XMMATRIX startingOrientation = XMMatrixRotationQuaternion(quat);
	XMStoreFloat4x4(&m_orientationMatrix, startingOrientation);

	UpdateImpl(0.f, 0.f, 0.f, 0.f, 0.f);
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
	float extraPitch = 0.f;
	float extraYaw = 0.f;
	float forwards = 0.f;
	float right = 0.f;

	if (m_gamepad && m_gamepad->IsConnected())
	{
		float lX = m_gamepad->GetLeftX();
		float lY = m_gamepad->GetLeftY();
		float rX = m_gamepad->GetRightX();
		float rY = m_gamepad->GetRightY();

		if (lX != 0.f || lY != 0.f || rX != 0.f || rY != 0.f)
		{
			float moveSpeedFactor = (5.f + (R2_MULTIPLIER*m_gamepad->GetR2()));

			extraPitch = (INVERT_Y ? rY : -rY) * ROTATIONS_PER_SECOND * dt;
			extraYaw = (INVERT_X ? -rX : rX) * ROTATIONS_PER_SECOND * dt;
			forwards = -lY * DISTANCE_PER_SECOND * dt * moveSpeedFactor;
			right = (INVERT_X ? -lX : lX) * DISTANCE_PER_SECOND * dt * moveSpeedFactor;
		}
	} else if (m_mouse) {
		bool lDown = m_mouse->IsDown(beMouse::Button::LeftButton);
		bool rDown = m_mouse->IsDown(beMouse::Button::RightButton);
		extraPitch = lDown ? m_mouse->GetYMovement() * MOUSE_SPEED_MULTIPLIER : 0.f;
		extraYaw = lDown ? m_mouse->GetXMovement() * MOUSE_SPEED_MULTIPLIER : 0.f;
		forwards = rDown ? m_mouse->GetYMovement() * MOUSE_SPEED_MULTIPLIER : 0.f;
		right = rDown ? m_mouse->GetXMovement() * MOUSE_SPEED_MULTIPLIER : 0.f;
	}
	UpdateImpl(dt, extraPitch, extraYaw, forwards, right);
}

void beFlightCamera::UpdateImpl(float dt, float extraPitch, float extraYaw, float extraForwards, float extraRight)
{
	m_pitch += extraPitch;
	m_pitch = beMath::Clamp(m_pitch, (float)-M_PI, 0.f);
	m_yaw += extraYaw;

	float yaw = m_yaw;
	// #TODO #CAMERA this used to do a whole lot of matrix stuff, is this better?
	XMMATRIX rotZ = XMMatrixRotationZ(yaw);
	XMMATRIX rotX = XMMatrixRotationX(m_pitch);
	XMMATRIX orientation = rotZ * rotX;
	orientation = XMMatrixInverse(nullptr, orientation);

	//cycles input
	XMVECTOR currentPosition = XMVectorSet(m_position.x, m_position.y, m_position.z, 1.f);
	XMVECTOR translation = XMVectorSet(extraRight, 0.f, extraForwards, 0.f);
	XMVECTOR transformedTranslation = XMVector3TransformNormal(translation, orientation);
	currentPosition += transformedTranslation;
	orientation.r[3] = currentPosition;

	XMMATRIX viewMatrix = XMMatrixInverse(nullptr, orientation);

	XMStoreFloat4x4(&m_orientationMatrix, orientation);
	XMStoreFloat4x4(&m_viewMatrix, viewMatrix);
	XMStoreFloat3(&m_position, currentPosition);

}

