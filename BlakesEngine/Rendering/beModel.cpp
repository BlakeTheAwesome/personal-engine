#include "BlakesEngine/bePCH.h"
#include "beModel.h"
#include "beRendering.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/DataStructures/beVector.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Rendering/beTexture.h"
#include "BlakesEngine/Shaders/beShaderTexture.h"
#include "BlakesEngine/Shaders/beShaderPack.h"
#include "BlakesEngine/Core/beStringUtil.h"

#include "BlakesEngine/DataStructures/beByteStreams.h"
#include "BlakesEngine/Core/beContainerHelpers.h"

#include <d3d11.h>

#include <fstream>
#include <charconv>
#include <filesystem>

import bePackedData;
import beMath;
import RangeIter;

using namespace beRendering;

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
	beHeapVector<Vec3, 2048> vertices;
	beHeapVector<Vec3, 2048> vertexNormals;
	beHeapVector<Vec2, 2048> texCoords;
	struct Mesh
	{
		beString meshName;
		beString material;
		beHeapVector<Face, 1024> faces;
		bool smoothShading = false;
	};
	beHeapVector<Mesh> meshes;
};

#pragma warning(push)
#pragma warning(disable:26481) // pointer arithmetic

std::optional<int> ReadInt(const char* begin, const char* end)
{
	bool success = true;
	int i1 = 0;
	auto result = std::from_chars(begin, end, i1);
	success |= (*result.ptr == ' ' || result.ptr == end);

	if (success)
	{
		return i1;
	}
	return {};
}

std::optional<float> ReadFloat(const char* begin, const char* end)
{
	bool success = true;
	float f1 = 0.f;
	auto result = std::from_chars(begin, end, f1);
	success |= (*result.ptr == ' ' || result.ptr == end);

	if (success)
	{
		return f1;
	}
	return {};
}

std::optional<Vec2> ReadVec2(const char* begin, const char* end)
{
	bool success = true;
	float f1 = 0.f;
	float f2 = 0.f;
	auto result = std::from_chars(begin, end, f1);
	success |= *result.ptr == ' ';
	result = std::from_chars(result.ptr+1, end, f2);
	success |= (*result.ptr == ' ' || result.ptr == end);

	if (success)
	{
		return Vec2{f1, f2};
	}
	return {};
}

std::optional<Vec3> ReadVec3(const char* begin, const char* end)
{
	bool success = true;
	float f1 = 0.f;
	float f2 = 0.f;
	float f3 = 0.f;
	auto result = std::from_chars(begin, end, f1);
	success |= *result.ptr == ' ';
	result = std::from_chars(result.ptr+1, end, f2);
	success |= *result.ptr == ' ';
	result = std::from_chars(result.ptr+1, end, f3);
	success |= (*result.ptr == ' ' || result.ptr == end);

	if (success)
	{
		return Vec3{f1, f2, f3};
	}
	return {};
}

template <int N, typename T, typename U>
static inline beString SubString(T lineStart, U lineEnd, const char(&)[N])
{
	auto lineStartPtr = lineStart + N;
	while (*lineStartPtr == ' ')
	{
		++lineStartPtr;
	}
	return beString(lineStart+N, lineEnd);
}

template <int N, typename T>
static inline const char* Offset(T lineStart, const char(&)[N])
{
	auto lineStartPtr = lineStart + N;
	while (*lineStartPtr == ' ')
	{
		++lineStartPtr;
	}
	return &(*lineStartPtr);
}

bool beModel::ReadMaterialLine(const std::string& line)
{
	auto begin = line.c_str();
	auto end = begin + line.size();

	if (beStringUtil::BeginsWith(begin, end, "newmtl"))
	{
		Material* material = m_materials.AddNew();
		material->m_name = SubString(begin, end, "newmtl");
		return true;
	}
	
	if (m_materials.Count() == 0)
	{
		return true;
	}

	Material* material = &m_materials.Last();
	if (beStringUtil::BeginsWith(begin, end, "Ka"))
	{
		if (auto res = ReadVec3(Offset(begin, "Ka"), end))
		{
			material->m_ambientColour = *res;
			return true;
		}
		BE_ASSERT(false);
		return false;
	}
	else if (beStringUtil::BeginsWith(begin, end, "Kd"))
	{
		if (auto res = ReadVec3(Offset(begin, "Kd"), end))
		{
			material->m_diffuseColour = *res;
			return true;
		}
		BE_ASSERT(false);
		return false;
	}
	else if (beStringUtil::BeginsWith(begin, end, "Ks"))
	{
		if (auto res = ReadVec3(Offset(begin, "Ks"), end))
		{
			material->m_specularColour = *res;
			return true;
		}
		BE_ASSERT(false);
		return false;
	}
	else if (beStringUtil::BeginsWith(begin, end, "Ns"))
	{
		if (auto f = ReadFloat(Offset(begin, "Ns"), end))
		{
			material->m_specularPower = f.value();
			return true;
		}
		BE_ASSERT(false);
		return false;
	}
	else if (beStringUtil::BeginsWith(begin, end, "d"))
	{
		if (auto f = ReadFloat(Offset(begin, "d"), end))
		{
			material->m_alpha = f.value();
			return true;
		}
		BE_ASSERT(false);
		return false;
	}
	else if (beStringUtil::BeginsWith(begin, end, "Tr"))
	{
		if (auto f = ReadFloat(Offset(begin, "Tr"), end))
		{
			material->m_alpha = 1.f - f.value();
			return true;
		}
		BE_ASSERT(false);
		return false;
	}
	else if (beStringUtil::BeginsWith(begin, end, "illum"))
	{
		if (auto i = ReadInt(Offset(begin, "illum"), end))
		{
			material->m_illuminationMode = i;
			return true;
		}
		BE_ASSERT(false);
		return false;
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
			auto subParamStart = Offset(begin, "map_Bump");
			auto subParamEnd = begin + lastSpaceIndex;
			while (subParamStart != subParamEnd)
			{
				while (*subParamStart == ' ') { subParamStart++; }

				if (beStringUtil::BeginsWith(subParamStart, subParamEnd, "-bm"))
				{
					if (auto f = ReadFloat(Offset(subParamStart, "-bm"), subParamEnd))
					{
						material->m_bumpMultiplier = f.value();
					}
					else
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

	const char* lineStartPtr = _line.c_str();
	const char* lineEndPtr = lineStartPtr + _line.size();
	auto lineStart = _line.begin();
	auto lineEnd = _line.end();



	if (beStringUtil::BeginsWith(lineStart, lineEnd, "o "))
	{
		if (currentMesh->faces.Count() > 0)
		{
			currentMesh = fileInfo->meshes.AddNew();
		}
		currentMesh->meshName = SubString(lineStart, lineEnd, "o");
		return true;
	}

	if (beStringUtil::BeginsWith(lineStart, lineEnd, "usemtl "))
	{
		currentMesh->material = SubString(lineStart, lineEnd, "usemtl");
		return true;
	}
	else if (beStringUtil::BeginsWith(lineStart, lineEnd, "vn "))
	{
		if (auto vec = ReadVec3(Offset(lineStart, "vn"), lineEndPtr))
		{
			fileInfo->vertexNormals.Insert(vec.value());
			return true;
		}
		return false;
	}
	else if (beStringUtil::BeginsWith(lineStart, lineEnd, "vt "))
	{
		if (auto vec = ReadVec2(Offset(lineStartPtr, "vt"), lineEndPtr))
		{
			fileInfo->texCoords.Insert(vec.value());
			return true;
		}
		return false;
	}
	else if (beStringUtil::BeginsWith(lineStart, lineEnd, "v "))
	{
		if (auto vec = ReadVec3(Offset(lineStartPtr, "v"), lineEndPtr))
		{
			//LOG("V - {:.3f}, {:.3f}, {:.3f}", vec->x, vec->y, vec->z);
			fileInfo->vertices.Insert(vec.value());
			return true;
		}
		return false;
	}
	else if (beStringUtil::BeginsWith(lineStart, lineEnd, "f "))
	{
		const char* next = Offset(lineStartPtr, "f");

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

#pragma warning(pop)

bool beModel::InitWithFilename(beRenderInterface* ri, beShaderPack* shaderPack, const char* filename, const beString& textureFilename, const beModel::LoadOptions& loadOptions)
{
	std::filesystem::path modelPath(filename);
	std::filesystem::path modelFolder = modelPath.parent_path();
	if (!LoadTexture(ri, shaderPack, textureFilename, modelFolder.string()))
	{
		BE_ASSERT(false);
		return false;
	}

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
		float x = VecElem(unswizzled, axesSwizzle.at(0));
		float y = VecElem(unswizzled, axesSwizzle.at(1));
		float z = VecElem(unswizzled, axesSwizzle.at(2));
		return Vec3(x, y, z);
	};
	
	for (const OBJFileInfo::Mesh& mesh : fileInfo.meshes)
	{
		indices.Clear();
		for (const Face& face : mesh.faces)
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

				//LOG("Before - {}", vertex);
				vert->position = ToVec4(swizzleVert(vertex), 1.f);
				vert->normal = normal;
				vert->uv = texCoord;

				//LOG("- {}", vert->position);
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

		auto outMesh = m_meshes.AddNew();
		outMesh->m_name = mesh.meshName;
		const beStringView& materialName = mesh.material;
		outMesh->m_materialIndex = m_materials.Count() == 1 ? 0 : m_materials.IndexOf([&](const Material& material) {return material.m_name == materialName; });
		bool success = outMesh->m_indexBuffer.Allocate(ri, ElementSize(indices), indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, loadOptions.topology, 0, 0, indices.begin());
		if (!success) { BE_ASSERT(false); return false; }
	}

	bool success = m_vertexBuffer.Allocate(ri, ElementSize(vertices), vertices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	return true;
}

bool beModel::InitFromBuffers(beRenderBuffer* vertexBuffer, std::span<Mesh> meshes, std::span<Material> materials)
{
	Deinit();

	m_vertexBuffer = std::move(*vertexBuffer);
	bool valid = m_vertexBuffer.IsValid();
	
	for (Material& iterMaterial : materials)
	{
		Material* material = m_materials.AddNew();
		*material = std::move(iterMaterial);
	}

	for (Mesh& iterMesh : meshes)
	{
		Mesh* mesh = m_meshes.AddNew();
		*mesh = std::move(iterMesh);
	}

	return valid;
}

bool beModel::VerifyHeader(bePackedData const& packedData)
{
	beDataBuffer headerBuffer = packedData.GetStream("main");
	if (headerBuffer.GetSize() != 2)
	{
		return false;
	}


	u8 magic = 0;
	u8 version = 0;
	beReadStream headerBufferStream(headerBuffer);
	headerBufferStream.Stream(magic);
	headerBufferStream.Stream(version);
	BE_ASSERT(magic == 0xef);
	if (version != 1)
	{
		return false;
	}

	return true;
}

bool beModel::InitFromPackedData(beRenderInterface* ri, beShaderPack* shaderPack, bePackedData const& packedData)
{
	Deinit();
	if (!VerifyHeader(packedData))
	{
		return false;
	}

	struct DrawCallInfo
	{
		s32 lodIndex;
		u32 vertexOffset;
		u32 vertexCount;
		u32 indexOffset;
		u32 indexCount;
		s32 shadowCapIndexCount;
		u32 materialIndex;
		u32 vertexFormatIndex;
		float boundingBox[6];
		int segmentVertexCount[5];
	};
	
	beVector<DrawCallInfo> drawCalls;
	{
		beReadStream drawCallsStream(packedData.GetStream("drawCalls"));
		u32 drawCallCount = 0; drawCallsStream >> drawCallCount;
		drawCalls.ReserveAndSetCountUninitialised(drawCallCount);
		drawCallsStream.Stream(drawCalls.begin(), drawCalls.Count() * sizeof(DrawCallInfo));
		if (drawCallsStream.HasFailed() || drawCallsStream.GetRemaining() != 0)
		{
			BE_ASSERT(false);
			return false;
		}
	}


	enum class VertexDataFormat : u16
	{
		Float = 0,
		Half,
		UByte,
		UInt12121212,
		SInt101111,
	};

	struct VertexInfo
	{
		beString label;
		VertexDataFormat type;
		u16 numElements;
	};

	using VertexSet = beVector<VertexInfo>;

	beVector<VertexSet> vertexFormats;
	{
		beReadStream vertexFormatStream(packedData.GetStream("formats"));
		u32 numVertexSets; vertexFormatStream >> numVertexSets;
		vertexFormats.Reserve(numVertexSets);
		while (numVertexSets--)
		{
			VertexSet& vertexSet = *vertexFormats.AddNew(); 
			u8 vertexFormatCount = 0; vertexFormatStream >> vertexFormatCount;
			std::generate_n(std::back_inserter(vertexSet), vertexFormatCount, 
				[&vertexFormatStream]() -> VertexInfo {
					u8 labelLen; vertexFormatStream >> labelLen;
					char labelBuffer[256];
					vertexFormatStream.Stream(labelBuffer, labelLen);
					labelBuffer[labelLen] = '\0';

					u16 type; vertexFormatStream >> type;
					u16 components; vertexFormatStream >> components;

					return {labelBuffer, VertexDataFormat{type}, components};
				}
			);
		}
	}




	beDataBuffer packedIndexBuffer = packedData.GetStream("IndexBuffer");
	beDataBuffer packedVertexBuffer = packedData.GetStream("VertexBuffer");

	if (packedIndexBuffer.GetSize() % 2 != 0 || packedIndexBuffer.GetSize() % 12 != 0)
	{
		BE_ASSERT(false);
		return false;
	}

	std::span<const u16> packedIndices(reinterpret_cast<const u16*>(packedIndexBuffer.GetBuffer()), packedIndexBuffer.GetSize() / sizeof(u16));
	std::span<const Vec3> packedVertices(reinterpret_cast<const Vec3*>(packedVertexBuffer.GetBuffer()), packedVertexBuffer.GetSize() / sizeof(Vec3));

	m_meshes.Reserve(drawCalls.Count());
	for (DrawCallInfo const& drawCall : drawCalls)
	{
		if (drawCall.vertexFormatIndex >= (u32)vertexFormats.Count())
		{
			BE_ASSERT(false);
			return false;
		}
		VertexSet const& vertexSet = vertexFormats[drawCall.vertexFormatIndex];

		VertexInfo const* positionInfo = std::find_if(vertexSet.begin(), vertexSet.end(), [](VertexInfo const& it) {
			return it.label == "Position";
		});
		if (!positionInfo)
		{
			BE_ASSERT(false && "No position vertex info");
			return false;
		}
		if (positionInfo->type != VertexDataFormat::Float || positionInfo->numElements != 3)
		{
			BE_ASSERT(false);
			return false;
		}

		VertexInfo const* normalInfo = std::find_if(vertexSet.begin(), vertexSet.end(), [](VertexInfo const& it) {
			return it.label == "Normal";
		});
		if (!normalInfo)
		{
			BE_ASSERT(false && "No normal vertex info");
			return false;
		}
		if (normalInfo->type != VertexDataFormat::Float || normalInfo->numElements != 3)
		{
			BE_ASSERT(false);
			return false;
		}


		u32 positionVertexOffset = drawCall.vertexOffset;
		u32 positionVertexCount = drawCall.vertexCount;
		u32 positionIndexOffset = drawCall.indexOffset;
		u32 positionIndexCount = drawCall.indexCount;
		//u32 normalVertexOffset = normalInfo->vertexOffset;
		//u32 normalVertexCount = normalInfo->vertexCount;
		//u32 normalIndexOffset = normalInfo->indexOffset;
		//u32 normalIndexCount = normalInfo->indexCount;
		//BE_ASSERT(positionVertexCount == normalVertexCount);
		//BE_ASSERT(positionVertexCount == normalIndexCount);

		BE_ASSERT(positionVertexOffset + positionVertexCount < packedVertices.size());
		//BE_ASSERT(normalVertexOffset + normalVertexCount) < packedVertices.size());
		BE_ASSERT(positionIndexOffset + positionIndexCount < packedIndices.size());
		//BE_ASSERT(normalIndexOffset + normalIndexCount) < packedIndices.size());


		beVector<VertexWithNormalType> vertices(positionVertexCount, positionVertexCount, 0);
		for (u32 i = 0; i < positionVertexCount; i++)
		{
			vertices[i].position = ToVec4(packedVertices[positionVertexOffset+i], 1.f);
			vertices[i].normal = packedVertices[positionVertexOffset+i]; //packedVertices[normalVertexOffset+i];
			vertices[i].uv = V20();
		}
		beVector<u32> indices(positionIndexCount, positionIndexCount, 0);
		for (u32 i = 0; i < positionIndexCount; i++)
		{
			u32 originalIndex = packedIndices[positionIndexOffset+i];
			BE_ASSERT(originalIndex < positionVertexCount);
			indices[i] = originalIndex;
		}

		bool success = m_vertexBuffer.Allocate(ri, ElementSize(vertices), vertices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
		if (!success) { BE_ASSERT(false); return false; }

		Mesh* mesh = m_meshes.AddNew();
		success = mesh->m_indexBuffer.Allocate(ri, ElementSize(indices), indices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, beRendering::Topology::TriangleList, 0, 0, indices.begin());
		if (!success) { BE_ASSERT(false); return false; }
		if (true) {
			break;
		}
	}

	return true;
}

bool beModel::Init(beRenderInterface* ri, beShaderPack* shaderPack, const beString& textureFilename)
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

	if (!LoadTexture(ri, shaderPack, textureFilename, ""))
	{
		BE_ASSERT(false);
		return false;
	}
	if (m_materials.Count() > 0)
	{
		BE_ASSERT(m_materials.Count() == 1);
		mesh->m_materialIndex = 0;
	}
	return true;
}

void beModel::Deinit()
{
	m_materials.Release();
	m_meshes.Release();
	m_vertexBuffer.Release();
}

void beModel::SetMeshVisibility(const beStringView& meshName, bool visible)
{
	Mesh* mesh = m_meshes.AddressOf([&](const Mesh& mesh) { return mesh.m_name == meshName; });
	BE_ASSERT(mesh);
	mesh->m_enabled = visible;
}

bool beModel::LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename, const beStringView& additionalLoadDir)
{
	m_materials.Release();

	beTexture::LoadOptions textureLoadOptions;
	//textureLoadOptions.format = beTextureFormat::R32G32B32_FLOAT;
	textureLoadOptions.format = beTextureFormat::R8G8B8A8_UNORM;
	textureLoadOptions.additionalLoadDir = additionalLoadDir.ToString();

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

	for (Material& material : m_materials)
	{
		if (!material.m_texDiffuse.empty())
		{
			if (!material.m_texture.Init(ri, shaderPack, material.m_texDiffuse, textureLoadOptions))
			{
				BE_ASSERT(false);
				return false;
			}
		}
	}
	return true;
}

void beModel::Render(beRenderInterface* ri, beShaderPack* shaderPack, beRendering::ShaderType shaderOverride)
{
	ID3D11DeviceContext* deviceContext = ri->GetDeviceContext();
	ID3D11Buffer* vertexBuffers[] = {m_vertexBuffer.GetBuffer()};
	u32 strides[] = {(u32)m_vertexBuffer.ElementSize()};
	u32 offsets[] = {0};
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffers[0], &strides[0], &offsets[0]);
	
	std::optional<beShaderLitTexture::ShaderParams> defaultLitParams;

	beRendering::ShaderType lastShaderType = ShaderType::Invalid;
	for (const Mesh& mesh : m_meshes)
	{
		if (!mesh.m_enabled)
		{
			continue;
		}
		const Material* material = mesh.m_materialIndex != -1 ? &m_materials[mesh.m_materialIndex] : nullptr;
		beRendering::ShaderType shader = shaderOverride;
		if (shader == ShaderType::Default && material)
		{
			shader = material->m_shader;
		}

		deviceContext->IASetIndexBuffer(mesh.m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)mesh.m_indexBuffer.D3DIndexTopology());
		switch (shader)
		{
			case beRendering::ShaderType::Texture:
			{
				if (lastShaderType != beRendering::ShaderType::Texture)
				{
					lastShaderType = beRendering::ShaderType::Texture;
					shaderPack->shaderTexture.SetActive(ri);
					shaderPack->shaderTexture.SetShaderParameters(ri);
				}
				shaderPack->shaderTexture.Render(ri, mesh.m_indexBuffer.NumElements(), material ? material->m_texture.GetTexture() : nullptr);
			}
			break;

			case beRendering::ShaderType::Colour:
			{
				if (lastShaderType != beRendering::ShaderType::Colour)
				{
					lastShaderType = beRendering::ShaderType::Colour;
					shaderPack->shaderColour.SetActive(ri);
					shaderPack->shaderColour.SetShaderParameters(ri);
				}
				shaderPack->shaderColour.Render(ri, mesh.m_indexBuffer.NumElements(), 0);
			}
			break;

			case beRendering::ShaderType::LitTexture:
			default:
			{
				if (lastShaderType != beRendering::ShaderType::LitTexture)
				{
					lastShaderType = beRendering::ShaderType::LitTexture;
					shaderPack->shaderLitTexture.SetActive(ri);
				}

				if (!defaultLitParams)
				{
					defaultLitParams = beShaderLitTexture::GetDefaultShaderParams(ri);
				}
				beShaderLitTexture::ShaderParams shaderParams = *defaultLitParams;
				shaderPack->shaderLitTexture.SetShaderParameters(ri, shaderParams);

				if (material)
				{
					if (material->m_ambientColour)
					{
						shaderParams.ambientColour = ToVec4(*material->m_ambientColour, 1.f);
					}
					if (material->m_diffuseColour)
					{
						shaderParams.diffuseColour = ToVec4(*material->m_diffuseColour, 1.f);
					}
					if (material->m_specularColour)
					{
						shaderParams.specularColour = ToVec4(*material->m_specularColour, 1.f);
					}
					if (material->m_specularPower)
					{
						shaderParams.specularPower = *material->m_specularPower;
					}
				}
				shaderPack->shaderLitTexture.Render(ri, mesh.m_indexBuffer.NumElements(), material ? material->m_texture.GetTexture() : nullptr);
			}
			break;
		}
	}

}

ID3D11ShaderResourceView* beModel::GetTexture(int meshIndex) const
{
	int materialIndex = m_meshes[meshIndex].m_materialIndex;
	return materialIndex >= 0 ? m_materials[materialIndex].m_texture.GetTexture() : nullptr;
}

int beModel::GetIndexCount(int meshIndex)
{
	return m_meshes[meshIndex].m_indexBuffer.NumElements();
}
