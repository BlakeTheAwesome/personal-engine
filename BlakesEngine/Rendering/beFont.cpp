#include "bePCH.h"
#include "beFont.h"

#include "Core\beAssert.h"
#include "Core\bePrintf.h"
#include "DataStructures\beVector.h"
#include "Rendering\beRenderInterface.h"
#include "Rendering\beTexture.h"

#include <d3d11.h>

#include <fstream>
#include <algorithm>

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
	beVector<Vec3> vertices;
	beVector<Vec3> vertexNormals;
	beVector<Vec2> texCoords;
	beVector<Face> faces;
};

struct VertexWithNormalType
{
	Vec4 position;
	Vec3 normal;
	Vec2 texCoord;
};

beFont::beFont()
	: m_characterInfo(128)
	, m_characterIndices(128)
	, m_extraKerning(8)
	, m_texture(nullptr)
{
	m_texture = new beTexture();
}

beFont::~beFont()
{
	BE_SAFE_DELETE(m_texture);
}

bool beFont::ReadLine(const std::string& line)
{
	// Ignore everything before the first semi-colon
	const char* next = &line[0];
	auto commentIndex = line.find_last_of(';');
	if (commentIndex != std::string::npos)
	{
		next = &line[commentIndex];
	}

	// ignore all whitespace before type character
	while (*next == ' ' || *next == ';')
	{
		next++;
	}

	// Type character will be C for a character definition, or K for a kerning pair
	if (*next == 'C')
	{
		u32 characterCode;
		int pixelWidth, preKern, postKern;
		float u1, v1, u2, v2;
		int res = sscanf_s(next, "C %u %f %f %f %f %d %d %d", &characterCode, &u1, &v1, &u2, &v2, &pixelWidth, &preKern, &postKern);
		if (res != 8)
		{
			return false;
		}
		// Careful with order here, we're getting the count before adding to the vector.
		u64 lookupIndex = ((u64)characterCode << 32) | (u64)m_characterInfo.Count();
		m_characterIndices.Insert(lookupIndex);

		auto characterInfo = m_characterInfo.AllocateNew();
		characterInfo->textureTopLeft = Vec2(u1, v1);
		characterInfo->textureBottomRight = Vec2(u2, v2);
		characterInfo->width = pixelWidth;
		characterInfo->prekerning = preKern;
		characterInfo->postkerning = postKern;
	}
	else if (*next == 'K')
	{
		u32 left, right;
		int kern;
		int res = sscanf_s(next, "K %u %u %d", &left, &right, &kern);
		if (res != 3)
		{
			return false;
		}
		auto extraKern = m_extraKerning.AllocateNew();
		extraKern->pair = (((u64)left) << 32) | (u64)right;
		extraKern->offset = kern;
	}

	return true;
}

bool beFont::Init(beRenderInterface* ri, const char* filename, const beWString& textureFilename)
{
	std::string line;
	std::ifstream fstream(filename);
	while (!fstream.eof())
	{
		std::getline(fstream, line);
		if (!ReadLine(line))
		{
			BE_ASSERT(false);
			return false;
		}
	}

	std::sort(m_characterIndices.begin(), m_characterIndices.end());

	/*for (auto entry : m_characterIndices)
	{
		u64 code = entry >> 32;
		u32 index = (u32)entry;
		const auto& info = m_characterInfo[index];
		bePRINTF("%lld: %c {%3.3f, %3.3f}, {%3.3f, %3.3f}, %d, %d, %d", code, (char)code, info.textureTopLeft.x, info.textureTopLeft.y, info.textureBottomRight.x, info.textureBottomRight.y, info.width, info.prekerning, info.postkerning);
	}*/

	/*for (auto entry : m_extraKerning)
	{
		u64 left = entry.pair >> 32;
		u32 right = (u32)entry.pair;
		bePRINTF("%lld (%c) %d (%c) %d", left, (char)left, right, (char)right, entry.offset);
	}*/
	
	return LoadTexture(ri, textureFilename);
}


void beFont::Deinit()
{
	m_texture->Deinit();
}

bool beFont::LoadTexture(beRenderInterface* ri, const beWString& textureFilename)
{
	return m_texture->Init(ri, textureFilename);
}

ID3D11ShaderResourceView * beFont::GetTexture() const
{
	return m_texture->GetTexture();
}
