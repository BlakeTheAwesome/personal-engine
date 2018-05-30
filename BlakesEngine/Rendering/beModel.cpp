#include "BlakesEngine/bePCH.h"
#include "beModel.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/DataStructures/beVector.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Rendering/beTexture.h"
#include "BlakesEngine/Shaders/beShaderTexture.h"

#include <d3d11.h>

#include <fstream>

using VertexWithNormalType = beShaderTexture::VertexType;

struct VertInfo
{
	int vertex;
	int uv;
	int normal;
};

struct Face
{
	VertInfo verts[3];
};

struct OBJFileInfo
{
	OBJFileInfo() : vertices(2048), vertexNormals(2048), texCoords(2048), faces(1024){}
	beVector<Vec3> vertices;
	beVector<Vec3> vertexNormals;
	beVector<Vec2> texCoords;
	beVector<Face> faces;
};

static bool ReadLine(const char* line, OBJFileInfo* fileInfo)
{
	if (line[0] == 'v' && line[1] == 'n')
	{
		float f1, f2, f3;
		int res = sscanf_s(line, "vn %f %f %f", &f1, &f2, &f3);
		if (res == 3)
		{
			Vec3 normal(f1,f2,f3);
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
		float f1, f2;
		int res = sscanf_s(line, "vt %f %f", &f1, &f2);
		if (res == 2)
		{
			Vec2 textureCoord(f1,f2);
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
		const char* next = line + 1;
		while (*next == ' ') { next++; }
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
			face->verts[i].uv = texCoord - 1;
			face->verts[i].normal = normal - 1;
		}
	}

	return true;
}

bool beModel::InitWithFilename(beRenderInterface* ri, beShaderPack* shaderPack, const char* filename, const beWString& textureFilename, const beModel::LoadOptions& loadOptions)
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
				BE_ASSERT(false);
				return false;
			}
		}
	}

	float scale = loadOptions.scale;

	int vertexCount = fileInfo.faces.Count() * 3; 
	int indexCount = vertexCount;
	
	beVector<VertexWithNormalType> vertices(vertexCount, vertexCount, 0);
	beVector<u32> indices(indexCount, indexCount, 0);

	for (int i : RangeIter(indices.Count()))
	{
		indices[i] = i;
	}

	int vertexIndex = 0;
	for (int i : RangeIter(fileInfo.faces.Count()))
	{
		const Face* face = &fileInfo.faces[i];
		//LOG("face %d", i);
		auto parseVert = [&](const VertInfo& vert)
		{
			Vec3 vertex = fileInfo.vertices[vert.vertex];
			Vec2 texCoord = (vert.uv == -1) ? V20() : fileInfo.texCoords[vert.uv];
			Vec3 normal = (vert.normal == -1) ? V3Z() : fileInfo.vertexNormals[vert.normal];
			vertices[vertexIndex].position = Vec4(vertex.x, vertex.y, vertex.z, 1.f) * scale;
			vertices[vertexIndex].normal = normal;
			vertices[vertexIndex].uv = texCoord;

			// Invert to swap rhs to lhs
			vertices[vertexIndex].position.z *= -1.f;
			vertices[vertexIndex].uv.y *= -1.f;
			vertices[vertexIndex].normal.z *= -1.f;
			//LOG("- %3.3f, %3.3f, %3.3f", vertices[vertexIndex].position.x, vertices[vertexIndex].position.y, vertices[vertexIndex].position.z);
			vertexIndex++;
		};

		if (loadOptions.flipFaces)
		{
			for (int j : RangeIterReverse(3))  // Read backwards to swap rhs to lhs
			{
				parseVert(face->verts[j]);
			}
		} 
		else
		{
			for (int j : RangeIter(3))
			{
				parseVert(face->verts[j]);
			}
		}
	}

	bool success = m_vertexBuffer.Allocate(ri, ElementSize(vertices), vertices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	success = m_indexBuffer.Allocate(ri, ElementSize(indices), indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 0, indices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	return LoadTexture(ri, shaderPack, textureFilename);
}

bool beModel::InitFromBuffers(beRenderBuffer* vertexBuffer, beRenderBuffer* indexBuffer)
{
	m_vertexBuffer = std::move(*vertexBuffer);
	m_indexBuffer = std::move(*indexBuffer);

	return m_vertexBuffer.IsValid() && m_indexBuffer.IsValid();
}

bool beModel::Init(beRenderInterface* ri, beShaderPack* shaderPack, const beWString& textureFilename)
{
	int vertexCount = 5; 
	int indexCount = vertexCount;
	
	beVector<VertexWithNormalType> vertices(vertexCount, vertexCount, 0);
	beVector<u32> indices(indexCount, indexCount, 0);
	for (auto& vertex : vertices)
	{
		vertex.normal = V3X();
	}
	
	// Load the vertex array with data.
	float distFromCamera = -1.f;
	float w = 1.f;
	vertices[0].position = Vec4(-1.f, 1.f, distFromCamera, w);  // TL
	vertices[0].uv = V20();
	
	vertices[1].position = Vec4(1.f, 1.f, distFromCamera, w);  // TR
	vertices[1].uv = V2X();

	vertices[2].position = Vec4(1.f, -1.f, distFromCamera, w);  // BR
	vertices[2].uv = V21();

	vertices[3].position = Vec4(-1.f, -1.f, distFromCamera, w);  // BL.
	vertices[3].uv = V2Y();

	vertices[4].position = Vec4(-1.f, 1.f, distFromCamera, w);  // TL.
	vertices[4].uv = V20();

	for (int i : RangeIter(indexCount))
	{
		indices[i] = i;
	}
	
	bool success = m_vertexBuffer.Allocate(ri, ElementSize(vertices), vertices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	success = m_indexBuffer.Allocate(ri, ElementSize(indices), indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 0, 0, indices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	return LoadTexture(ri, shaderPack, textureFilename);
}

void beModel::Deinit()
{
	m_texture.Deinit();
	m_indexBuffer.Release();
	m_vertexBuffer.Release();
}

bool beModel::LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beWString& textureFilename)
{
	beTexture::LoadOptions textureLoadOptions;
	//textureLoadOptions.format = beTextureFormat::R32G32B32_FLOAT;
	textureLoadOptions.format = beTextureFormat::R8G8B8A8_UNORM;
	return m_texture.Init(ri, shaderPack, textureFilename, textureLoadOptions);
}

void beModel::Render(beRenderInterface* ri)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	ID3D11Buffer* vertexBuffers[] = {m_vertexBuffer.GetBuffer()};
	u32 strides[] = {(u32)m_vertexBuffer.ElementSize()};
	u32 offsets[] = {0};

	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	deviceContext->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_indexBuffer.D3DIndexTopology());
}

ID3D11ShaderResourceView * beModel::GetTexture() const
{
	return m_texture.GetTexture();
}

int beModel::GetIndexCount()
{
	return m_indexBuffer.NumElements();
}
