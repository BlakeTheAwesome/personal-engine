#include "bePCH.h"
#include "beFlightCamera.h"

#include "Core/beAssert.h"
#include "Input/beGamepad.h"

static const float ROTATIONS_PER_SECOND = 1.0f;
static const float DISTANCE_PER_SECOND = 1.0f;

beFlightCamera::beFlightCamera()
	: m_pos(0.0f, 1.0f, -5.0f)
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

	/*float extraPitch = m_gamepad->GetRightY() * ROTATIONS_PER_SECOND * dt * 360.0f;
	float extraYaw = m_gamepad->GetRightX() * ROTATIONS_PER_SECOND * dt * 360.0f;
	m_rot.x = fmodf(m_rot.x + extraYaw, 259.0f);
	m_rot.y = fmodf(m_rot.y + extraPitch, 259.0f);
	

	float forwards = m_gamepad->GetLeftY() * ROTATIONS_PER_SECOND * dt * 360.0f;
	m_pos = XMVectorAdd(
	float extraYaw = m_gamepad->GetRightX() * ROTATIONS_PER_SECOND * dt * 360.0f;
	m_rot.x = fmodf(m_rot.x + extraYaw, 259.0f);
	m_rot.y = fmodf(m_rot.y + extraPitch, 259.0f);
	
	
	
	float pitch = DEG_TO_RAD(m_rot.x);
	float yaw = DEG_TO_RAD(m_rot.y);
	float roll = DEG_TO_RAD(m_rot.z);

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	Vec3 up(0.0f, 1.0f, 0.0f);
	Vec3 lookAt(0.0f, 0.0f, 1.0f);
	
	XMVECTOR xUp = XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix);
	XMVECTOR xLookAt = XMVector3TransformCoord(XMLoadFloat3(&lookAt), rotationMatrix);
	
	XMMATRIX xViewMat = XMMatrixLookAtLH(XMLoadFloat3(&m_pos), xLookAt, xUp);
	XMStoreFloat4x4(&m_matrix, xViewMat);*/
}

