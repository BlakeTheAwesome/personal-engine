#include "BlakesEngine/bePCH.h"
#include "beModel.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/DataStructures/beVector.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Rendering/beTexture.h"
#include "BlakesEngine/Shaders/beShaderTexture.h"
#include "BlakesEngine/Core/beStringUtil.h"

#include <d3d11.h>

#include <fstream>
#include <charconv>

using VertexWithNormalType = beShaderTexture::VertexType;

struct VertInfo
{
	int vertex;
	int uv;
	int normal;
};

struct Face
{
	beFixedVector<VertInfo, 4> verts;
};

struct beModel::OBJFileInfo
{
	beVector<Vec3> vertices{2048};
	beVector<Vec3> vertexNormals{2048};
	beVector<Vec2> texCoords{2048};
	struct Mesh
	{
		beString material;
		beVector<Face> faces{1024};
		bool smoothShading = false;
	};
	beVector<Mesh> meshes;
};

std::optional<Vec3> ReadVec3(const char* begin, const char* end)
{
	// #BJLPTODO: next MSVC will have std::from_chars float implementation, would be better. 
	float f1 = 0.f;
	float f2 = 0.f;
	float f3 = 0.f;
	int res = sscanf_s(begin, "%f %f %f", &f1, &f2, &f3);
	if (res != 3)
	{
		return {};
	}
	return Vec3(f1, f2, f3);
}

bool beModel::ReadMaterialLine(const std::string& line)
{
	auto begin = line.c_str();
	auto end = begin + line.size();

	if (beStringUtil::BeginsWith(begin, end, "newmtl"))
	{
		Material* material = m_materials.AddNew();
		material->m_name = beString(begin + 7, end);
		return true;
	}
	
	if (m_materials.Count() == 0)
	{
		return true;
	}

	Material* material = &m_materials.Last();
	if (beStringUtil::BeginsWith(begin, end, "Ka"))
	{
		auto res = ReadVec3(begin + 3, end);
		if (!res)
		{
			BE_ASSERT(false);
			return false;
		}

		material->m_ambientColour = *res;
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "Kd"))
	{
		auto res = ReadVec3(begin + 3, end);
		if (!res)
		{
			BE_ASSERT(false);
			return false;
		}

		material->m_diffuseColour = *res;
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "Ks"))
	{
		auto res = ReadVec3(begin + 3, end);
		if (!res)
		{
			BE_ASSERT(false);
			return false;
		}

		material->m_specularColour = *res;
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "Ns"))
	{
		int res = sscanf_s(begin + 3, "%f", &material->m_specExponent);
		if (res != 1)
		{
			BE_ASSERT(false);
			return false;
		}
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "d"))
	{
		float alpha = 0.f;
		int res = sscanf_s(begin + 2, "%f", &alpha);
		if (res != 1)
		{
			BE_ASSERT(false);
			return false;
		}
		material->m_alpha = alpha;
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "Tr"))
	{
		float transparency = 0.f;
		int res = sscanf_s(begin + 3, "%f", &transparency);
		if (res != 1)
		{
			BE_ASSERT(false);
			return false;
		}
		material->m_alpha = 1.f - transparency;
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "illum"))
	{
		int res = sscanf_s(begin + 6, "%d", &material->m_illuminationMode);
		if (res != 1)
		{
			BE_ASSERT(false);
			return false;
		}
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "map_Kd "))
	{
		auto lastSpaceIndex = line.rfind(" ");
		material->m_texDiffuse = line.substr(lastSpaceIndex+1);
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "map_Ka "))
	{
		auto lastSpaceIndex = line.rfind(" ");
		material->m_texAmbient = line.substr(lastSpaceIndex+1);
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "map_Ks "))
	{
		auto lastSpaceIndex = line.rfind(" ");
		material->m_texSpec = line.substr(lastSpaceIndex+1);
		return true;
	}
	else if (beStringUtil::BeginsWith(begin, end, "map_Bump "))
	{
		auto lastSpaceIndex = line.rfind(" ");
		material->m_texBump = line.substr(lastSpaceIndex+1);

		if (lastSpaceIndex != 8)
		{
			auto subParamStart = begin + 8;
			auto subParamEnd = begin + lastSpaceIndex;
			while (subParamStart != subParamEnd)
			{
				subParamStart++;
				if (beStringUtil::BeginsWith(subParamStart, subParamEnd, "-bm"))
				{
					if (sscanf_s(subParamStart, "-bm %f", &material->m_bumpMultiplier) != 1)
					{
						return false;
					}
				}

				subParamStart = std::find(subParamStart, subParamEnd, ' ');
			}
		}
		return true;
	}

	return true;
}

bool beModel::ReadMeshLine(const std::string& _line, OBJFileInfo* fileInfo)
{
	if (fileInfo->meshes.Count() == 0)
	{
		fileInfo->meshes.AddNew();
	}
	OBJFileInfo::Mesh* currentMesh = &fileInfo->meshes.Last();

	const char* line = _line.c_str();
	auto lineStart = _line.begin();
	auto lineEnd = _line.end();

	if (beStringUtil::BeginsWith(lineStart, lineEnd, "o "))
	{
		if (currentMesh->faces.Count() > 0)
		{
			currentMesh = fileInfo->meshes.AddNew();
		}
		currentMesh->material = _line.substr(2);
		return true;
	}

	if (beStringUtil::BeginsWith(lineStart, lineEnd, "vn "))
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
	else if (beStringUtil::BeginsWith(lineStart, lineEnd, "vt "))
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
	else if (beStringUtil::BeginsWith(lineStart, lineEnd, "v "))
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
	else if (beStringUtil::BeginsWith(lineStart, lineEnd, "f "))
	{
		const char* next = line + 1;
		while (*next == ' ') { next++; }

		Face* face = currentMesh->faces.AddNew();

		const char* start = next;
		const char* chunkEnd = start;
		const char* end = start + strlen(start);
			
		while (start != end)
		{
			chunkEnd = std::find(start, end, ' ');
			int pos = -1;
			int texcoord = 1;
			int normal = -1;
			auto result = std::from_chars(start, chunkEnd, pos);
			BE_ASSERT(*result.ptr == '/');
			result = std::from_chars(result.ptr+1, chunkEnd, texcoord);
			BE_ASSERT(*result.ptr == '/');
			result = std::from_chars(result.ptr+1, chunkEnd, normal);
			BE_ASSERT(*result.ptr == ' ' || result.ptr == end);

			VertInfo* vert = face->verts.AddNew();
			BE_ASSERT(pos != 0);
			vert->vertex = pos - 1;
			vert->uv = texcoord - 1;
			vert->normal = normal - 1; 
			
			start = result.ptr;
			if (*start == ' ')
			{
				start++;
			}
		}
	}
	else if (beStringUtil::BeginsWith(lineStart, lineEnd, "s "))
	{
		currentMesh->smoothShading = beStringUtil::EndsWith(lineStart, lineEnd, "1");
	}

	return true;
}

bool beModel::InitWithFilename(beRenderInterface* ri, beShaderPack* shaderPack, const char* filename, const beStringView& textureFilename, const beModel::LoadOptions& loadOptions)
{
	OBJFileInfo fileInfo;
	{
		std::string line;
		std::ifstream fstream(filename);
		while (!fstream.eof())
		{
			std::getline(fstream, line);
			if (!ReadMeshLine(line.c_str(), &fileInfo))
			{
				BE_ASSERT(false);
				return false;
			}
		}
	}

	float scale = loadOptions.scale;

	int numFaces = fileInfo.meshes.First().faces.Count();
	int maxVertCount = numFaces * 4;
	
	beVector<VertexWithNormalType> vertices(maxVertCount);
	beVector<u32> indices(maxVertCount);

	const auto& axesSwizzle = loadOptions.axesSwizzle;
	auto swizzleVert = [axesSwizzle](Vec3 unswizzled)
	{
		float x = VecElem(unswizzled, axesSwizzle[0]);
		float y = VecElem(unswizzled, axesSwizzle[1]);
		float z = VecElem(unswizzled, axesSwizzle[2]);
		return Vec3(x, y, z);
	};
	
	for (const Face& face : fileInfo.meshes.First().faces)
	{
		u32 vertIndex = vertices.Count();
		int numVerts = face.verts.Count();
		for (int i : RangeIter(numVerts))
		{
			const VertInfo& vertInfo = face.verts[i];

			VertexWithNormalType* vert = vertices.AddNew();
			Vec3 vertex = fileInfo.vertices[vertInfo.vertex] * scale;
			Vec3 normal = (vertInfo.normal == -1) ? V3Z() : fileInfo.vertexNormals[vertInfo.normal];
			Vec2 texCoord = (vertInfo.uv == -1) ? V20() : fileInfo.texCoords[vertInfo.uv];
			
			// Invert to swap rhs to lhs
			vertex.z *= -1.f;
			normal.z *= -1.f;
			texCoord.y *= -1.f;

			vert->position = beMath::ToVec4(swizzleVert(vertex), 1.f);
			vert->normal = normal;
			vert->uv = texCoord;

			//LOG("- %3.3f, %3.3f, %3.3f", vertices[vertexIndex].position.x, vertices[vertexIndex].position.y, vertices[vertexIndex].position.z);
		}

		enum IndexOrder
		{
			Invalid,
			Tri_lhs,
			Tri_rhs,
			Quad_lhs,
			Quad_rhs,
		} order = Invalid;

		if (numVerts == 3)
		{
			order = loadOptions.flipFaces ? Tri_lhs : Tri_rhs;
		}
		else if (numVerts == 4)
		{
			order = loadOptions.flipFaces ? Quad_lhs : Quad_rhs;
		}

		switch (order)
		{
			case Tri_lhs: { indices.AddRange({vertIndex+2,vertIndex+1,vertIndex+0}); } break;
			case Tri_rhs: { indices.AddRange({vertIndex+0,vertIndex+1,vertIndex+2}); } break;
			case Quad_lhs: { indices.AddRange({vertIndex+2,vertIndex+1,vertIndex+0}); indices.AddRange({vertIndex+0,vertIndex+3,vertIndex+2}); } break;
			case Quad_rhs: { indices.AddRange({vertIndex+0,vertIndex+1,vertIndex+2}); indices.AddRange({vertIndex+0,vertIndex+2,vertIndex+3}); } break;
			default: BE_ASSERT(false);
		}
	}

	bool success = m_vertexBuffer.Allocate(ri, ElementSize(vertices), vertices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	Mesh* mesh = m_meshes.AddNew();
	success = mesh->m_indexBuffer.Allocate(ri, ElementSize(indices), indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, loadOptions.topology, 0, 0, indices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	return LoadTexture(ri, shaderPack, textureFilename);
}

bool beModel::InitFromBuffers(beRenderBuffer* vertexBuffer, beRenderBuffer* indexBuffer)
{
	Deinit();

	m_vertexBuffer = std::move(*vertexBuffer);
	Mesh* mesh = m_meshes.AddNew();
	mesh->m_indexBuffer = std::move(*indexBuffer);

	return m_vertexBuffer.IsValid() && mesh->m_indexBuffer.IsValid();
}

bool beModel::Init(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename)
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

	Mesh* mesh = m_meshes.AddNew();
	success = mesh->m_indexBuffer.Allocate(ri, ElementSize(indices), indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, beRendering::Topology::TriangleStrip, 0, 0, indices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	return LoadTexture(ri, shaderPack, textureFilename);
}

void beModel::Deinit()
{
	m_materials.Release();
	m_meshes.Release();
	m_vertexBuffer.Release();
}

bool beModel::LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename)
{
	m_materials.Release();

	beTexture::LoadOptions textureLoadOptions;
	//textureLoadOptions.format = beTextureFormat::R32G32B32_FLOAT;
	textureLoadOptions.format = beTextureFormat::R8G8B8A8_UNORM;

	if (!beStringUtil::EndsWithI(textureFilename, "mtl"))
	{
		// Load single texture file
		Material* material = m_materials.AddNew();
		return material->m_texture.Init(ri, shaderPack, textureFilename, textureLoadOptions);
	}

	{
		std::string line;
		std::ifstream fstream(textureFilename.c_str());
		while (!fstream.eof())
		{
			std::getline(fstream, line);
			if (!ReadMaterialLine(line))
			{
				BE_ASSERT(false);
				return false;
			}
		}
	}

	return false;
}

void beModel::Render(beRenderInterface* ri)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	ID3D11Buffer* vertexBuffers[] = {m_vertexBuffer.GetBuffer()};
	u32 strides[] = {(u32)m_vertexBuffer.ElementSize()};
	u32 offsets[] = {0};

	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	deviceContext->IASetIndexBuffer(m_meshes[0].m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_meshes[0].m_indexBuffer.D3DIndexTopology());
}

ID3D11ShaderResourceView * beModel::GetTexture(int materialIndex) const
{
	return m_materials[materialIndex].m_texture.GetTexture();
}

int beModel::GetIndexCount(int meshIndex)
{
	return m_meshes[meshIndex].m_indexBuffer.NumElements();
}
