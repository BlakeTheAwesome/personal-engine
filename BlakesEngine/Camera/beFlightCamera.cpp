#include "bePCH.h"
#include "beFlightCamera.h"

#include "Core/beAssert.h"
#include "Core/bePrintf.h"
#include "Input/beGamepad.h"

static const float ROTATIONS_PER_SECOND = 0.5f * (2.f * PI);
static const float DISTANCE_PER_SECOND = 1.0f;
static const bool INVERT_Y = true;

beFlightCamera::beFlightCamera()
	: m_gamepad(NULL)
{
	XMVECTOR startingPosition = XMVectorSet(-5.f, 0.f, 0.f, 0.f);
	XMVECTOR startingLookat = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	XMVECTOR startingUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMMATRIX startingMatrix = XMMatrixLookAtLH(startingPosition, startingLookat, startingUp);
	XMStoreFloat4x4(&m_matrix, startingMatrix);
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

	float moveSpeedFactor = (1.f + m_gamepad->GetR2());

	float extraPitch = (INVERT_Y ? -rY : rY) * ROTATIONS_PER_SECOND * dt;
	float extraYaw = -rX * ROTATIONS_PER_SECOND * dt;
	float forwards = -lX * DISTANCE_PER_SECOND * dt * moveSpeedFactor;
	float right = -lY * DISTANCE_PER_SECOND * dt * moveSpeedFactor;

	XMMATRIX initialMatrix = XMLoadFloat4x4(&m_matrix);
	XMMATRIX rotX = XMMatrixRotationX(extraPitch);
	XMMATRIX rotY = XMMatrixRotationY(extraYaw);
	XMMATRIX translation = XMMatrixTranslation(forwards, 0.f, right);
	XMMATRIX newMatrix = initialMatrix * rotX * rotY * translation;

	XMStoreFloat4x4(&m_matrix, newMatrix);
}

