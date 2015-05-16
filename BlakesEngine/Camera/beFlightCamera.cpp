#include "bePCH.h"
#include "beFlightCamera.h"

#include "Core/beAssert.h"
#include "Core/bePrintf.h"
#include "Input/beGamepad.h"

static const float ROTATIONS_PER_SECOND = 1.0f;
static const float DISTANCE_PER_SECOND = 1.0f;

beFlightCamera::beFlightCamera()
	: m_pos(-5.0f, 1.0f, 0.0f)
	, m_rot(0.0f, -.5f, 0.0f, 0.0f)
	, m_gamepad(NULL)
{
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
	m_gamepad = NULL;
}

const Matrix& beFlightCamera::GetViewMatrix() const
{
	return m_matrix;
}

void beFlightCamera::Update(float dt)
{
	if (!m_gamepad)
	{
		return;
	}

	float lX = m_gamepad->GetLeftX();
	float lY = m_gamepad->GetLeftY();
	float rX = m_gamepad->GetRightX();
	float rY = m_gamepad->GetRightY();

	if (lX == 0.f && lY == 0.f && rX == 0.f && rY == 0.f)
	{
		return;
	}

	float extraPitch = rY * ROTATIONS_PER_SECOND * dt * 360.0f;
	float extraYaw = rX * ROTATIONS_PER_SECOND * dt * 360.0f;
	m_rot.x = fmodf(m_rot.x + extraYaw, 359.0f);
	m_rot.y = fmodf(m_rot.y + extraPitch, 359.0f);
	

	float forwards = lY * DISTANCE_PER_SECOND * dt * 360.0f;
	float right = -lX * DISTANCE_PER_SECOND * dt * 360.0f;
	
	float pitch = DEG_TO_RAD(m_rot.x);
	float yaw = DEG_TO_RAD(m_rot.y);
	float roll = DEG_TO_RAD(m_rot.z);

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	Vec3 up(0.0f, 1.0f, 0.0f);
	Vec3 lookAt(1.0f, 0.0f, 0.0f);
	
	XMVECTOR xUp = XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix);
	XMVECTOR xLookAt = XMVector3TransformCoord(XMLoadFloat3(&lookAt), rotationMatrix);
	XMVECTOR xRight = XMVector3Cross(xUp, xLookAt);
	XMVECTOR xCurrentPos = XMLoadFloat3(&m_pos);
	XMVECTOR newPos = xCurrentPos + (xLookAt * forwards) + (xRight * right);

	XMMATRIX xViewMat = XMMatrixLookAtLH(newPos, xLookAt, xUp);
	XMStoreFloat4x4(&m_matrix, xViewMat);

	XMFLOAT3 oldPos = m_pos;
	XMStoreFloat3(&m_pos, newPos);
	XMFLOAT3 lookat; XMStoreFloat3(&lookat, xLookAt);
	XMFLOAT3 vRight; XMStoreFloat3(&vRight, xRight);
	bePRINTF("Old pos %3.3f, %3.3f, %3.3f\n + %3.3f * (%3.3f, %3.3f, %3.3f)\n + %3.3f * (%3.3f, %3.3f, %3.3f)\n = (%3.3f, %3.3f, %3.3f)", 
		oldPos.x, oldPos.y, oldPos.z,
		forwards, lookat.x, lookat.y, lookat.z,
		right, vRight.x, vRight.y, vRight.z,
		m_pos.x, m_pos.y, m_pos.z);
}

