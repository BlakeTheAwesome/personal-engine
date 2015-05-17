#include "bePCH.h"
#include "beModel.h"

#include "Core\beAssert.h"
#include "Core\bePrintf.h"
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
	OBJFileInfo() : vertices(2048), vertexNormals(2048), texCoords(2048), faces(1024){}
	beVector<XMFLOAT3> vertices;
	beVector<XMFLOAT3> vertexNormals;
	beVector<XMFLOAT2> texCoords;
	beVector<Face> faces;
};


struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
};

struct VertexWithNormalType
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
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
	else if (line[0] == 'v' && line[1] == 't')
	{
		float f1, f2, f3;
		int res = sscanf_s(line, "vt %f %f %f", &f1, &f2, &f3);
		if (res == 3)
		{
			XMFLOAT2 textureCoord(f1,f2);
			fileInfo->texCoords.Insert(textureCoord);
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
		const char* next = line + 2;
		Face* face = fileInfo->faces.AllocateNew();
		for (int i = 0; i < 3; i++)
		{
			// .OBJ is 1 indexed, so 0 here will be -1 for us.
			int pos = 0;
			int texCoord = 0;
			int normal = 0;

			int buffIndex = 0;
			int nextThing = 0;
			char buff[32];
			while (true)
			{
				if (*next >= '0' && *next <= '9')
				{
					BE_ASSERT(buffIndex < 32);
					buff[buffIndex++] = *next;
					next++;
					continue;
				}
				buff[buffIndex] = 0;
				if (*buff != 0)
				{
					int thing = atoi(buff);
					if (nextThing == 0)
					{
						pos = thing;
					}
					else if (nextThing == 1)
					{
						texCoord = thing;
					}
					else
					{
						normal = thing;
					}
				}
				nextThing++;
				buffIndex = 0;

				if (*next++ != '/')
				{
					break;
				}
			}
			
			BE_ASSERT(pos != 0);
			face->verts[i].vertex = pos - 1;
			face->verts[i].texCoord = texCoord - 1;
			face->verts[i].normal = normal - 1;
		}
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
	VertexWithNormalType* vertices = NULL;
	unsigned int* indices;
	HRESULT res;

	ID3D11Device* device = ri->GetDevice();


	m_vertexCount = fileInfo.faces.Count() * 3; 
	//m_vertexCount += 9; // Add an extra 3 debug tris

	m_indexCount = m_vertexCount;
	
	vertices = new VertexWithNormalType[m_vertexCount];
	indices = new unsigned int[m_indexCount];
	int vertexIndex = 0;
	for (int i = 0; i < fileInfo.faces.Count(); i++)
	{
		const Face* face = &fileInfo.faces[i];
		bePRINTF("face %d", i);
		for (int j = 2; j >= 0; j--) // Read backwards to swap rhs to lhs
		{
			const VertInfo* vert = &face->verts[j];
			XMFLOAT3 vertex = fileInfo.vertices[vert->vertex];
			XMFLOAT2 texCoord = (vert->texCoord == -1) ? XMFLOAT2(0.0f, 0.0f) : fileInfo.texCoords[vert->texCoord];
			XMFLOAT3 normal = (vert->normal == -1) ? XMFLOAT3(0.0f, 1.0f, 0.0f) : fileInfo.vertexNormals[vert->normal];
			vertices[vertexIndex].position = vertex;
			vertices[vertexIndex].normal = normal;
			vertices[vertexIndex].texCoord = texCoord;

			// Invert to swap rhs to lhs
			vertices[vertexIndex].position.z *= -1.f;
			vertices[vertexIndex].texCoord.y *= -1.f;
			vertices[vertexIndex].normal.z *= -1.f;
			bePRINTF("- %3.3f, %3.3f, %3.3f", vertices[vertexIndex].position.x, vertices[vertexIndex].position.y, vertices[vertexIndex].position.z);
			vertexIndex++;
		}
	}

	// Debug tris
	/*for (int i = 9; i > 0; i--)
	{
		vertices[m_vertexCount -i].texCoord = XMFLOAT2(0.0f, 0.0f);
	}
	vertices[m_vertexCount -9].position = XMFLOAT3(0.f, 0.f, 0.f);
	vertices[m_vertexCount -8].position = XMFLOAT3(1.f, 0.f, 0.f);
	vertices[m_vertexCount -7].position = XMFLOAT3(0.f, 1.f, 0.f);
	vertices[m_vertexCount -9].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[m_vertexCount -8].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[m_vertexCount -7].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	
	vertices[m_vertexCount -6].position = XMFLOAT3(0.f, 0.f, 0.f);
	vertices[m_vertexCount -5].position = XMFLOAT3(1.f, 0.f, 0.f);
	vertices[m_vertexCount -4].position = XMFLOAT3(0.f, 0.f, 1.f);
	vertices[m_vertexCount -6].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[m_vertexCount -5].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[m_vertexCount -4].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	
	vertices[m_vertexCount -3].position = XMFLOAT3(0.f, 0.f, 0.f);
	vertices[m_vertexCount -2].position = XMFLOAT3(0.f, 1.f, 0.f);
	vertices[m_vertexCount -1].position = XMFLOAT3(0.f, 0.f, 1.f);
	vertices[m_vertexCount -3].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[m_vertexCount -2].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[m_vertexCount -1].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);*/


	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexWithNormalType) * m_vertexCount;
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
