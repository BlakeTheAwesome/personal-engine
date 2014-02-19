#include "bePCH.h"
#include "beModel.h"

#include "Core\beAssert.h"
#include "DataStructures\beVector.h"
#include "Rendering\beRenderInterface.h"
#include "Rendering\beTexture.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include <fstream>

struct VertInfo
{
	int vertex;
	int texCoord;
	int normal;
};

struct Face
{
	VertInfo verts[3];
};

struct OBJFileInfo
{
	OBJFileInfo() : vertices(2048), vertexNormals(2048), faces(1024){}
	beVector<XMFLOAT3> vertices;
	beVector<XMFLOAT3> vertexNormals;
	beVector<Face> faces;
};


struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
};

beModel::beModel()
	: m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
	, m_vertexCount(0)
	, m_indexCount(0)
{
	m_texture = new beTexture();
}

beModel::~beModel()
{
	BE_ASSERT(!m_vertexBuffer && !m_indexBuffer);
	BE_SAFE_DELETE(m_texture);
}

static bool ReadLine(const char* line, OBJFileInfo* fileInfo)
{
	if (line[0] == 'v' && line[1] == 'n')
	{
		float f1, f2, f3;
		int res = sscanf_s(line, "vn %f %f %f", &f1, &f2, &f3);
		if (res == 3)
		{
			XMFLOAT3 normal(f1,f2,f3);
			fileInfo->vertexNormals.Insert(normal);
			return true;
		}
		else
		{
			return false;
		} 
	}
	else if (line[0] == 'v')
	{
		float f1, f2, f3;
		int res = sscanf_s(line, "v %f %f %f", &f1, &f2, &f3);
		if (res == 3)
		{
			XMFLOAT3 vert(f1,f2,f3);
			fileInfo->vertices.Insert(vert);
			return true;
		}
		else
		{
			return false;
		} 
	}
	else if (line[0] == 'f')
	{
		// sscanf when there is no middle number?
		//f 45//27 35//26 24//35
	}

	return true;
}

bool beModel::InitWithFilename(beRenderInterface* ri, const char* filename)
{
	OBJFileInfo fileInfo;
	{
		std::string line;
		std::ifstream fstream(filename);
		while (!fstream.eof())
		{
			std::getline(fstream, line);
			if (!ReadLine(line.c_str(), &fileInfo))
			{
				return false;
			}
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {0};
	D3D11_BUFFER_DESC indexBufferDesc = {0};
	D3D11_SUBRESOURCE_DATA vertexData = {0};
	D3D11_SUBRESOURCE_DATA indexData = {0};
	VertexType* vertices = NULL;
	unsigned int* indices;
	HRESULT res;

	ID3D11Device* device = ri->GetDevice();

	m_vertexCount = 6;
	m_indexCount = 6;

	vertices = new VertexType[m_vertexCount];
	indices = new unsigned int[m_indexCount];
	
	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3A(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texCoord = XMFLOAT2A(0.0f, 1.0f);
	
	vertices[1].position = XMFLOAT3A(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[1].texCoord = XMFLOAT2A(0.0f, 0.0f);

	vertices[2].position = XMFLOAT3A(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[2].texCoord = XMFLOAT2A(1.0f, 0.0f);

	vertices[3].position = XMFLOAT3A(1.0f, 1.0f, 0.0f);  // TR.
	vertices[3].texCoord = XMFLOAT2A(1.0f, 0.0f);
	
	vertices[4].position = XMFLOAT3A(1.0f, -1.0f, 0.0f);  // BR.
	vertices[4].texCoord = XMFLOAT2A(1.0f, 1.0f);

	vertices[5].position = XMFLOAT3A(-1.0f, -1.0f, 0.0f);  // BL.
	vertices[5].texCoord = XMFLOAT2A(0.0f, 1.0f);



	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	delete [] vertices;
	delete [] indices;

	return true;
}

bool beModel::Init(beRenderInterface* ri)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {0};
	D3D11_BUFFER_DESC indexBufferDesc = {0};
	D3D11_SUBRESOURCE_DATA vertexData = {0};
	D3D11_SUBRESOURCE_DATA indexData = {0};
	VertexType* vertices = NULL;
	unsigned int* indices;
	HRESULT res;

	ID3D11Device* device = ri->GetDevice();

	m_vertexCount = 6;
	m_indexCount = 6;

	vertices = new VertexType[m_vertexCount];
	indices = new unsigned int[m_indexCount];
	
	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3A(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texCoord = XMFLOAT2A(0.0f, 1.0f);
	
	vertices[1].position = XMFLOAT3A(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[1].texCoord = XMFLOAT2A(0.0f, 0.0f);

	vertices[2].position = XMFLOAT3A(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[2].texCoord = XMFLOAT2A(1.0f, 0.0f);

	vertices[3].position = XMFLOAT3A(1.0f, 1.0f, 0.0f);  // TR.
	vertices[3].texCoord = XMFLOAT2A(1.0f, 0.0f);
	
	vertices[4].position = XMFLOAT3A(1.0f, -1.0f, 0.0f);  // BR.
	vertices[4].texCoord = XMFLOAT2A(1.0f, 1.0f);

	vertices[5].position = XMFLOAT3A(-1.0f, -1.0f, 0.0f);  // BL.
	vertices[5].texCoord = XMFLOAT2A(0.0f, 1.0f);



	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(res)) { BE_ASSERT(false); return false; }

	delete [] vertices;
	delete [] indices;

	return true;
}

void beModel::Deinit()
{
	m_texture->Deinit();
	BE_SAFE_RELEASE(m_indexBuffer);
	BE_SAFE_RELEASE(m_vertexBuffer);
}

bool beModel::LoadTexture(beRenderInterface* ri, const beWString& textureFilename)
{
	return m_texture->Init(ri, textureFilename);
}

void beModel::Render(beRenderInterface* ri)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int beModel::GetIndexCount()
{
	return m_indexCount;
}
