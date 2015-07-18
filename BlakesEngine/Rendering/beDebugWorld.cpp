#include "bePCH.h"
#include "beDebugWorld.h"

#include "BlakesEngine\Core\beAssert.h"
#include "BlakesEngine\Core\bePrintf.h"
#include "BlakesEngine\DataStructures\beVector.h"
#include "beRenderInterface.h"
#include "beRenderBuffer.h"
#include "BlakesEngine\Shaders/beShaderColour.h"
#include "BlakesEngine/Math/beRandom.h"
#include "BlakesEngine/Math/bePerlinNoise.h"

#include <d3d11.h>

#include <fstream>


struct VertexColourType
{
	Vec4 position;
	Vec4 colour;
};

PIMPL_DATA(beDebugWorld)
	bool InitAxes(beRenderInterface* ri);
	bool InitGrid(beRenderInterface* ri)
		;
	beRenderBuffer axesVertexBuffer;
	beRenderBuffer axesIndexBuffer;
	
	beRenderBuffer gridVertexBuffer;
	beRenderBuffer gridLinesIndexBuffer;
	beRenderBuffer gridFilledIndexBuffer;
	bool renderAxes;
	bool renderGrid;
PIMPL_DATA_END


PIMPL_CONSTRUCT(beDebugWorld)
	: renderAxes(false)
	, renderGrid(true)
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
	success |= self.InitGrid(ri);
	return success;
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
	int vertexCount = 6;

	beVector<VertexColourType> vertices(vertexCount, vertexCount, 0);
	beVector<u32> indices(vertexCount, vertexCount, 0);
	
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


	bool success = axesVertexBuffer.Allocate(ri, decltype(vertices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	success = axesIndexBuffer.Allocate(ri, decltype(indices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, indices.begin());
	if (!success) { BE_ASSERT(false); return false; }
	
	return true;
}

bool beDebugWorld::Impl::InitGrid(beRenderInterface* ri)
{
	int gridRadius = 100;
	float gridSize = 1.f;
	float xzOffset = gridSize / 2.f; // Don't draw on same spot as renderAxes
	float gridOffset = ((float)gridRadius / 2.f);

	int quadCount = gridRadius * gridRadius;
	int vertexCount = quadCount * 8;
	int triCount = quadCount * 2;
	int triVertexCount = triCount * 3;


	float noiseScale = 5.f;
	float noiseHeight = 8.f;
	beRandom rng;
	rng.InitFromSystemTime();
	bePerlinNoise2D noise;
	noise.Initialise(rng.Next());

	beVector<VertexColourType> vertices(vertexCount, vertexCount, 0);
	beVector<u32> lineIndices(vertexCount, vertexCount, 0);
	beVector<u32> triIndices(triVertexCount, triVertexCount, 0);
	
	for (int i = 0; i < lineIndices.Count(); i++)
	{
		lineIndices[i] = i;
	}

	for (int i = 0; i < quadCount; i++)
	{
		int triListIndex = i * 6;
		int lineListIndex = i * 8;
		triIndices[triListIndex+0] = lineListIndex+0;
		triIndices[triListIndex+1] = lineListIndex+2;
		triIndices[triListIndex+2] = lineListIndex+4;
		
		triIndices[triListIndex+3] = lineListIndex+4;
		triIndices[triListIndex+4] = lineListIndex+6;
		triIndices[triListIndex+5] = lineListIndex+0;
	}

	int vertexIndex = 0;
	for (float x = -gridOffset; x < gridOffset; x += gridSize)
	{
		float xPos0 = x + xzOffset;
		float xPos1 = xPos0+gridSize;
		for (float z = -gridOffset; z < gridOffset; z += gridSize)
		{
			float zPos0 = z + xzOffset;
			float zPos1 = zPos0+gridSize;

			float yPos0 = noiseHeight * noise.GetOctave(xPos0/noiseScale, zPos0/noiseScale, 4);
			float yPos1 = noiseHeight * noise.GetOctave(xPos0/noiseScale, zPos1/noiseScale, 4);
			float yPos2 = noiseHeight * noise.GetOctave(xPos1/noiseScale, zPos1/noiseScale, 4);
			float yPos3 = noiseHeight * noise.GetOctave(xPos1/noiseScale, zPos0/noiseScale, 4);
			Vec4 pos0(xPos0, yPos0, zPos0, 1.f);
			Vec4 pos1(xPos0, yPos1, zPos1, 1.f);
			Vec4 pos2(xPos1, yPos2, zPos1, 1.f);
			Vec4 pos3(xPos1, yPos3, zPos0, 1.f);

			vertices[vertexIndex+0].position = pos0;
			vertices[vertexIndex+1].position = pos1;
			vertices[vertexIndex+2].position = pos1;
			vertices[vertexIndex+3].position = pos2;
			vertices[vertexIndex+4].position = pos2;
			vertices[vertexIndex+5].position = pos3;
			vertices[vertexIndex+6].position = pos3;
			vertices[vertexIndex+7].position = pos0;
			vertices[vertexIndex+0].colour = Vec4(yPos0 / noiseHeight, yPos0 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+1].colour = Vec4(yPos1 / noiseHeight, yPos1 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+2].colour = Vec4(yPos1 / noiseHeight, yPos1 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+3].colour = Vec4(yPos2 / noiseHeight, yPos2 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+4].colour = Vec4(yPos2 / noiseHeight, yPos2 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+5].colour = Vec4(yPos3 / noiseHeight, yPos3 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+6].colour = Vec4(yPos3 / noiseHeight, yPos3 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+7].colour = Vec4(yPos0 / noiseHeight, yPos0 / noiseHeight, 1.f, 1.f);
			vertexIndex += 8;
		}
	}

	bool success = gridVertexBuffer.Allocate(ri, decltype(vertices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	success = gridLinesIndexBuffer.Allocate(ri, decltype(lineIndices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, lineIndices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	success = gridFilledIndexBuffer.Allocate(ri, decltype(triIndices)::element_size, triVertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, triIndices.begin());
	if (!success) { BE_ASSERT(false); return false; }
	
	return true;
}

void beDebugWorld::Render(beRenderInterface* ri, beShaderColour* shaderColour, const Matrix& viewMatrix, const Vec3& cameraPosition)
{
	if (self.renderGrid)
	{
		shaderColour->SetShaderParameters(ri, viewMatrix);

		ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
		unsigned int stride = self.gridVertexBuffer.ElementSize();
		unsigned int offset = 0;

		ID3D11Buffer* vertexBuffers[] = {self.gridVertexBuffer.GetBuffer()};
		deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

		deviceContext->IASetIndexBuffer(self.gridFilledIndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		shaderColour->Render(ri, self.gridFilledIndexBuffer.NumElements(), 0);

		deviceContext->IASetIndexBuffer(self.gridLinesIndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		shaderColour->Render(ri, self.gridLinesIndexBuffer.NumElements(), 0);
	}
	if (self.renderAxes)
	{
		ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
		unsigned int stride = self.axesVertexBuffer.ElementSize();
		unsigned int offset = 0;

		ID3D11Buffer* vertexBuffers[] = {self.axesVertexBuffer.GetBuffer()};
		deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
		deviceContext->IASetIndexBuffer(self.axesIndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		shaderColour->Render(ri, self.axesIndexBuffer.NumElements(), 0);
	}
}

void beDebugWorld::SetRenderAxes(bool v)
{
	self.renderAxes = v;
}

void beDebugWorld::SetRenderGrid(bool v)
{
	self.renderGrid = v;
}
