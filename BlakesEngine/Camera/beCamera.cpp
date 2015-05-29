#include "bePCH.h"
#include "beCamera.h"

beCamera::beCamera()
	: m_pos(0.0f, 0.0f, -5.0f)
	, m_rot(0.0f, -.5f, 0.0f)
{
}

beCamera::~beCamera()
{
}

void beCamera::SetPosition(const Vec3& pos)
{
	m_pos = pos;
}

void beCamera::SetRotation(const Vec3& rot)
{
	m_rot = rot;
}

const Vec3& beCamera::GetPosition() const
{
	return m_pos;
}

const Vec3& beCamera::GetRotation() const
{
	return m_rot;
}

const Matrix& beCamera::GetViewMatrix() const
{
	return m_matrix;
}

void beCamera::Update()
{
	float pitch = DEG_TO_RAD(m_rot.x);
	float yaw = DEG_TO_RAD(m_rot.y);
	float roll = DEG_TO_RAD(m_rot.z);

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	Vec3 up(0.0f, 1.0f, 0.0f);
	Vec3 lookAt(0.0f, 0.0f, 1.0f);
	
	XMVECTOR xUp = XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix);
	XMVECTOR xLookAt = XMVector3TransformCoord(XMLoadFloat3(&lookAt), rotationMatrix);
	
	XMMATRIX xViewMat = XMMatrixLookAtLH(XMLoadFloat3(&m_pos), xLookAt, xUp);
	XMStoreFloat4x4(&m_matrix, xViewMat);
}

