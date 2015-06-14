#include "bePCH.h"
#include "beFlightCamera.h"

#include "BlakesEngine\Core/beAssert.h"
#include "BlakesEngine\Core/bePrintf.h"
#include "BlakesEngine\Input/beGamepad.h"

static const float ROTATIONS_PER_SECOND = 0.5f * (2.f * PI);
static const float DISTANCE_PER_SECOND = 1.0f;
static const bool INVERT_Y = true;



//OK blake you need to render your axes pronto, like simple mesh with a colour per axis
//Could even go for three basic quads with a different colour shader on each.


beFlightCamera::beFlightCamera()
	: m_gamepad(nullptr)
	, m_position(0.f, 0.f, -5.f)
	, m_yaw(0.f)
	, m_pitch(0.f)
	, m_roll(0.f)
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

const Vec3& beFlightCamera::GetPosition() const
{
	return m_position;
}

const Matrix& beFlightCamera::GetViewMatrix() const
{
	return m_viewMatrix;
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

	float extraPitch = rX * ROTATIONS_PER_SECOND * dt;
	float extraYaw = (INVERT_Y ? rY : -rY) * ROTATIONS_PER_SECOND * dt;
	float forwards = lY * DISTANCE_PER_SECOND * dt * moveSpeedFactor;
	float right = lX * DISTANCE_PER_SECOND * dt * moveSpeedFactor;
	UpdateImpl(dt, extraPitch, extraYaw, forwards, right);
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

	m_yaw += extraYaw;
	m_pitch += extraPitch;
	m_roll += 0.f;

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(m_yaw, m_pitch, m_roll);
	XMMATRIX orientation = XMMatrixRotationQuaternion(quat);

	//member floats that temporarily contain the current

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

	XMVECTOR scale = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	XMVECTOR origin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMMATRIX newMat = XMMatrixAffineTransformation(scale, origin, quat, currentPosition);


	//the orientation matrix describes a view that stares directly at

	//the camera, invert it to get your view matrix.

	XMMATRIX viewMatrix = XMMatrixInverse(nullptr, newMat);

	XMStoreFloat4x4(&m_orientationMatrix, orientation);
	XMStoreFloat4x4(&m_viewMatrix, viewMatrix);
	XMStoreFloat3(&m_position, currentPosition);

}

