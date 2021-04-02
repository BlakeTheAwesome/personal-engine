module;

#include "DirectXMath.h"

export module beDirectXMath;

export namespace DirectXMath
{
	export using XMFLOAT2 = DirectX::XMFLOAT2;
	export using XMFLOAT3 = DirectX::XMFLOAT3;
	export using XMFLOAT4 = DirectX::XMFLOAT4;
	export using XMFLOAT4A = DirectX::XMFLOAT4A;
	export using XMFLOAT4X4 = DirectX::XMFLOAT4X4;
	export using XMVECTOR = DirectX::XMVECTOR;
	export using XMMATRIX = DirectX::XMMATRIX;

	export auto XMLoadFloat3 = DirectX::XMLoadFloat3;
	export auto XMStoreFloat3 = DirectX::XMStoreFloat3;
	export auto XMLoadFloat4A = DirectX::XMLoadFloat4A;
	export auto XMStoreFloat4A = DirectX::XMStoreFloat4A;
	export auto XMLoadFloat4x4 = DirectX::XMLoadFloat4x4;
	export auto XMStoreFloat4x4 = DirectX::XMStoreFloat4x4;
	export auto XMVectorAdd = DirectX::XMVectorAdd;
	export auto XMVectorSubtract = DirectX::XMVectorSubtract;
	export auto XMVectorScale = DirectX::XMVectorScale;
	export auto XMVector3Normalize = DirectX::XMVector3Normalize;
	export auto XMVector3TransformCoord = DirectX::XMVector3TransformCoord;
	export auto XMMatrixRotationRollPitchYaw = DirectX::XMMatrixRotationRollPitchYaw;
	export auto XMMatrixLookAtLH = DirectX::XMMatrixLookAtLH;

	export auto XMQuaternionRotationRollPitchYaw = DirectX::XMQuaternionRotationRollPitchYaw;
	export auto XMMatrixRotationQuaternion = DirectX::XMMatrixRotationQuaternion;
	export auto XMMatrixRotationZ = DirectX::XMMatrixRotationZ;
	export auto XMMatrixRotationX = DirectX::XMMatrixRotationX;
	export auto XMMatrixInverse = DirectX::XMMatrixInverse;
	export auto XMVectorSet = DirectX::XMVectorSet;
	export auto XMVectorGetByIndex = DirectX::XMVectorGetByIndex;
	export auto XMVectorReplicate = DirectX::XMVectorReplicate;
	export auto XMMatrixMultiply= DirectX::XMMatrixMultiply;
	export auto XMVectorMultiplyAdd = DirectX::XMVectorMultiplyAdd;
	export auto XMVector3UnprojectStream = DirectX::XMVector3UnprojectStream;
	export auto XMVector3TransformNormal = DirectX::XMVector3TransformNormal;
	export auto XMVector3Greater = DirectX::XMVector3Greater;
	export auto XMVector3Less = DirectX::XMVector3Less;
	export auto XMMatrixPerspectiveFovRH = DirectX::XMMatrixPerspectiveFovRH;
	export auto XMMatrixIdentity = DirectX::XMMatrixIdentity;
	export auto XMMatrixOrthographicRH = DirectX::XMMatrixOrthographicRH;
	export auto XMMatrixTranslationFromVector = DirectX::XMMatrixTranslationFromVector;
	export auto XMMatrixTranspose = DirectX::XMMatrixTranspose;
}
