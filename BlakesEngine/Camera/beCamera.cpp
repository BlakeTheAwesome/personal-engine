#include "BlakesEngine/bePCH.h"
#include "beCamera.h"

import beDirectXMath;
using namespace DirectXMath;

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
	const float pitch = DEG_TO_RAD(m_rot.x);
	const float yaw = DEG_TO_RAD(m_rot.y);
	const float roll = DEG_TO_RAD(m_rot.z);

	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	const Vec3 up(0.0f, 0.0f, 1.0f);
	const Vec3 lookAt(1.0f, 0.0f, 0.0f);
	
	const XMVECTOR xUp = XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix);
	const XMVECTOR xLookAt = XMVector3TransformCoord(XMLoadFloat3(&lookAt), rotationMatrix);
	
	const XMMATRIX xViewMat = XMMatrixLookAtLH(XMLoadFloat3(&m_pos), xLookAt, xUp);
	XMStoreFloat4x4(&m_matrix, xViewMat);
}

