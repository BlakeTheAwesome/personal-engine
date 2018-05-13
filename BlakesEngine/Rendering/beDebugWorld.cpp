#include "BlakesEngine/bePCH.h"
#include "beDebugWorld.h"
#include "beRenderInterface.h"
#include "beRenderBuffer.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/DataStructures/beArray.h"
#include "BlakesEngine/DataStructures/beVector.h"
#include "BlakesEngine/Shaders/beShaderColour.h"
#include "BlakesEngine/Math/beRandom.h"
#include "BlakesEngine/Math/bePerlinNoise.h"
#include "BlakesEngine/Framework/beAppData.h"
#include "BlakesEngine/Camera/beCameraUtils.h"
#include "BlakesEngine/Input/beMouse.h"

#include <d3d11.h>

#include <fstream>

using VertexColourType = beShaderColour::VertexType;

PIMPL_DATA(beDebugWorld)
	bool InitAxes(beRenderInterface* ri);

	beRenderBuffer axesVertexBuffer;
	beRenderBuffer axesIndexBuffer;
	beRenderBuffer mouseVertexBuffer;
	beRenderBuffer mouseIndexBuffer;
	
	beRenderBuffer gridVertexBuffer;
	beRenderBuffer gridLinesIndexBuffer;
	beRenderBuffer gridFilledIndexBuffer;
	bool renderAxes = false;
	bool renderMouse = false;
	bool haveMouseVerts = false;
PIMPL_DATA_END


PIMPL_CONSTRUCT(beDebugWorld)
{
}

PIMPL_DESTROY(beDebugWorld)
{
	BE_ASSERT(!axesVertexBuffer.IsValid() && !axesIndexBuffer.IsValid());
}

bool beDebugWorld::Init(beRenderInterface* ri)
{
	bool success = true;
	success |= self.InitAxes(ri);
	return success;
}

void beDebugWorld::Update(const beAppData& appData, const Matrix& viewMatrix)
{
	if (self.renderMouse)
	{
		auto mouse = appData.mouse;
		auto ri = appData.renderInterface;

		static constexpr int maxVertexCount = 18;
		beArray<VertexColourType, maxVertexCount> vertices;
		beArray<u32, maxVertexCount> indices; for (int i : RangeIter(maxVertexCount)) { indices[i] = i; }
		int vertCount = 0;

		Vec4 startColour(1.f, 1.f, 1.f, 1.f);
		Vec4 endColour(1.f, 1.f, 0.f, 0.5f);

		Vec3 pos, dir;
		auto addPos = [&](float x, float y)
		{
			if (beCameraUtils::GetScreeenToWorldRay(*ri, viewMatrix, x, y, &pos, &dir))
			{
				Vec3 endPoint3 = pos + (dir * 100.f);
				vertices[vertCount] = VertexColourType{beMath::ToVec4(pos, 1.f), startColour};
				vertices[vertCount+1] = VertexColourType{beMath::ToVec4(endPoint3, 1.f), endColour};
				vertCount += 2;
			}
		};

		float mouseX = (float)mouse->GetX();
		float mouseY = (float)mouse->GetY();
		addPos(mouseX-5, mouseY-5);
		addPos(mouseX-5, mouseY  );
		addPos(mouseX-5, mouseY+5);
		addPos(mouseX  , mouseY-5);
		addPos(mouseX  , mouseY  );
		addPos(mouseX  , mouseY+5);
		addPos(mouseX+5, mouseY-5);
		addPos(mouseX+5, mouseY  );
		addPos(mouseX+5, mouseY+5);

		self.haveMouseVerts = vertCount > 0;

		LOG("MouseX:{:.2f} mouseY{:.2f}\r\n", mouseX, mouseY);

		if (self.haveMouseVerts)
		{
			bool success = self.mouseVertexBuffer.Allocate(ri, decltype(vertices)::element_size, vertCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
			if (!success) { BE_ASSERT(false); }

			success = self.mouseIndexBuffer.Allocate(ri, decltype(indices)::element_size, vertCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, 0, 0, indices.begin());
			if (!success) { BE_ASSERT(false); }
		}
	}
}

void beDebugWorld::Deinit()
{
	self.gridFilledIndexBuffer.Release();
	self.gridLinesIndexBuffer.Release();
	self.gridVertexBuffer.Release();

	self.axesIndexBuffer.Release();
	self.axesVertexBuffer.Release();
}

bool beDebugWorld::Impl::InitAxes(beRenderInterface* ri)
{
	enum
	{
		vertexCount = 6,
	};

	beArray<VertexColourType, vertexCount> vertices;
	beArray<u32, vertexCount> indices;
	
	// Load the vertex array with data.
	float AxisLength = 5.f;
	float w = 1.f;
	vertices[0].position = Vec4(0.f, 0.f, 0.f, w);
	vertices[0].colour = Vec4(1.f, 0.f, 0.f, 1.f);
	vertices[1].position = Vec4(AxisLength, 0.f, 0.f, w);
	vertices[1].colour = Vec4(1.f, 0.f, 0.f, 1.f);

	vertices[2].position = Vec4(0.f, 0.f, 0.f, w);
	vertices[2].colour = Vec4(0.f, 1.f, 0.f, 1.f);
	vertices[3].position = Vec4(0.f, AxisLength, 0.f, w);
	vertices[3].colour = Vec4(0.f, 1.f, 0.f, 1.f);

	vertices[4].position = Vec4(0.f, 0.f, 0.f, w);
	vertices[4].colour = Vec4(0.f, 0.f, 1.f, 1.f);
	vertices[5].position = Vec4(0.f, 0.f, AxisLength, w);
	vertices[5].colour = Vec4(0.f, 0.f, 1.f, 1.f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;


	bool success = axesVertexBuffer.Allocate(ri, decltype(vertices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	success = axesIndexBuffer.Allocate(ri, decltype(indices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, 0, 0, indices.begin());
	if (!success) { BE_ASSERT(false); return false; }
	
	return true;
}

void beDebugWorld::ToggleRenderMouseRay()
{
	self.renderMouse = !self.renderMouse;
}

void beDebugWorld::Render(beRenderInterface* ri, beShaderColour* shaderColour, const Matrix& viewMatrix, const Vec3& cameraPosition)
{
	if (self.renderAxes)
	{
		ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
		ID3D11Buffer* vertexBuffers[] = {self.axesVertexBuffer.GetBuffer()};
		u32 strides[] = {(u32)self.axesVertexBuffer.ElementSize()};
		u32 offsets[] = {0};
		deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
		deviceContext->IASetIndexBuffer(self.axesIndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)self.axesIndexBuffer.D3DIndexTopology());

		shaderColour->Render(ri, self.axesIndexBuffer.NumElements(), 0);
	}

	if (self.renderMouse && self.haveMouseVerts)
	{
		ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
		ID3D11Buffer* vertexBuffers[] = {self.mouseVertexBuffer.GetBuffer()};
		u32 strides[] = {(u32)self.mouseVertexBuffer.ElementSize()};
		u32 offsets[] = {0};
		deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
		deviceContext->IASetIndexBuffer(self.mouseIndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)self.mouseIndexBuffer.D3DIndexTopology());

		shaderColour->Render(ri, self.mouseIndexBuffer.NumElements(), 0);
	}
}

void beDebugWorld::SetRenderAxes(bool v)
{
	self.renderAxes = v;
}
