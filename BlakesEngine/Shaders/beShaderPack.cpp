module;

#include "BlakesEngine/bePCH.h"
#include "beShaderCBufferDefinitions.h"
#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/beMacros.h"

#include <d3d11.h>

module beShaderPack;

import beRenderInterface;
import beDirectXMath;
using namespace DirectXMath;

struct beShaderPack::FrameData
{
	beShaderDefinitions::MatrixBuffer m_matrixData;
	beRenderBuffer m_matrixBuffer;
	
	beShaderDefinitions::OrthoMatrixBuffer m_orthoMatrixData;
	beRenderBuffer m_orthoMatrixBuffer;

	beShaderDefinitions::CameraBuffer m_cameraData;
	beRenderBuffer m_cameraBuffer;
};

beShaderPack::beShaderPack()
{
	m_frameData = std::make_unique<FrameData>();
}

beShaderPack::~beShaderPack()
{
	BE_ASSERT(!m_frameData->m_matrixBuffer.IsValid());
}

void beShaderPack::Init(beRenderInterface* ri)
{
	bool success = m_frameData->m_matrixBuffer.Allocate(ri, sizeof(beShaderDefinitions::MatrixBuffer), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	Unused(success);
	BE_ASSERT(success);

	success = m_frameData->m_orthoMatrixBuffer.Allocate(ri, sizeof(beShaderDefinitions::OrthoMatrixBuffer), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	BE_ASSERT(success);

	success = m_frameData->m_cameraBuffer.Allocate(ri, sizeof(beShaderDefinitions::CameraBuffer), 1, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, 0, D3D11_CPU_ACCESS_WRITE, 0, nullptr);
	BE_ASSERT(success);
}

void beShaderPack::Deinit()
{
	m_frameData->m_matrixBuffer.Release();
	m_frameData->m_orthoMatrixBuffer.Release();
	m_frameData->m_cameraBuffer.Release();
}

void beShaderPack::UpdateFrameBuffers(beRenderInterface* ri, const Matrix& viewMatrix)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();

	{
		XMMATRIX xWM = XMLoadFloat4x4(&ri->GetWorldMatrix());
		XMMATRIX xVM = XMLoadFloat4x4(&viewMatrix);
		XMMATRIX xPM = XMLoadFloat4x4(&ri->GetProjectionMatrix());

		XMMATRIX txWorldMatrix = XMMatrixTranspose(xWM);
		XMMATRIX txViewMatrix = XMMatrixTranspose(xVM);
		XMMATRIX txProjectionMatrix = XMMatrixTranspose(xPM);
		XMMATRIX txViewToProj = XMMatrixMultiply(txProjectionMatrix, txViewMatrix);
		XMMATRIX txWorldToProj = XMMatrixMultiply(txViewToProj, txWorldMatrix);

		Matrix world, view, projection, worldToProj;
		XMStoreFloat4x4(&world, txWorldMatrix);
		XMStoreFloat4x4(&view, txViewMatrix);
		XMStoreFloat4x4(&projection, txProjectionMatrix);
		XMStoreFloat4x4(&worldToProj, txWorldToProj);

		auto* current = &m_frameData->m_matrixData;
		bool update = false;
		update = update || (world != current->worldMatrix);
		update = update || (view != current->viewMatrix);
		update = update || (projection != current->projectionMatrix);

		if (update)
		{
			current->worldMatrix = world;
			current->viewMatrix = view;
			current->projectionMatrix = projection;
			current->worldToScreenMatrix = worldToProj;

			auto dataPtr = (beShaderDefinitions::MatrixBuffer*)m_frameData->m_matrixBuffer.Map(ri);
			if (!dataPtr) {  BE_ASSERT(false); return; }
			*dataPtr = *current;
			m_frameData->m_matrixBuffer.Unmap(ri);

			ID3D11Buffer* constantBuffers[] = {m_frameData->m_matrixBuffer.GetBuffer()};
			deviceContext->VSSetConstantBuffers(CBuffers::MatrixBuffer, 1, constantBuffers);
		}
	}

	{
		const Matrix& orthoMatrix = ri->GetOrthoMatrix();
		const Matrix& orthoMatrixPixelSpace = ri->GetOrthoMatrixPixelCoord();
		const Vec2& screenSize = ri->GetScreenSize();
		auto* current = &m_frameData->m_orthoMatrixData;
		bool update = false;
		update = update || orthoMatrix != current->orthoMatrixNormalised;
		update = update || orthoMatrixPixelSpace != current->orthoMatrixPixels;
		update = update || screenSize != current->screenSize;

		if (update)
		{
			current->orthoMatrixNormalised = orthoMatrix;
			current->orthoMatrixPixels = orthoMatrixPixelSpace;
			current->screenSize = screenSize;

			auto dataPtr = (beShaderDefinitions::OrthoMatrixBuffer*)m_frameData->m_orthoMatrixBuffer.Map(ri);
			if (!dataPtr)
			{
				return;
			}
			*dataPtr = *current;
			m_frameData->m_orthoMatrixBuffer.Unmap(ri);

			ID3D11Buffer* vsConstantBuffers[] ={m_frameData->m_orthoMatrixBuffer.GetBuffer()};
			deviceContext->VSSetConstantBuffers(CBuffers::OrthoMatrixBuffer, 1, vsConstantBuffers);
		}
	}

	{
		Vec3 cameraPosition = PositionFromMatrix(viewMatrix);
		auto* current = &m_frameData->m_cameraData;
		bool update = cameraPosition != current->cameraPosition;

		if (update)
		{
			current->cameraPosition = cameraPosition;

			auto dataPtr = (beShaderDefinitions::CameraBuffer*)m_frameData->m_cameraBuffer.Map(ri);
			if (!dataPtr)
			{
				return;
			}
			*dataPtr = *current;
			m_frameData->m_cameraBuffer.Unmap(ri);

			ID3D11Buffer* vsConstantBuffers[] = {m_frameData->m_cameraBuffer.GetBuffer()};
			deviceContext->VSSetConstantBuffers(CBuffers::CameraBuffer, 1, vsConstantBuffers);
		}
	}
}
