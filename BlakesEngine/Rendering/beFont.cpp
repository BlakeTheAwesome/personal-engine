#include "BlakesEngine/bePCH.h"
#include "beFont.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/DataStructures/beVector.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Rendering/beTexture.h"

#include <d3d11.h>

#include <fstream>
#include <algorithm>
#include <string>

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

	// Type character will be F for font definition, C for a character definition, or K for a kerning pair
	if (*next == 'F')
	{
		int lineHeight;
		int res = sscanf_s(next, "F %d", &lineHeight);
		if (res != 1)
		{
			return false;
		}
		m_lineHeight = lineHeight;
	}
	else if (*next == 'C')
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
		characterInfo->textureBtmRight = Vec2(u2, v2);
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
	while (fstream && !fstream.eof())
	{
		std::getline(fstream, line);
		if (!ReadLine(line))
		{
			BE_ASSERT(false);
			return false;
		}
	}

	if (!fstream)
	{
		char errmsg[256];
		strerror_s(errmsg, sizeof(errmsg), errno);
		bePRINTF("Error %s", errmsg);
		return false;
	}

	std::sort(m_characterIndices.begin(), m_characterIndices.end());

	/*for (auto entry : m_characterIndices)
	{
		u64 code = entry >> 32;
		u32 index = (u32)entry;
		const auto& info = m_characterInfo[index];
		bePRINTF("%lld: %c {%3.3f, %3.3f}, {%3.3f, %3.3f}, %d, %d, %d", code, (char)code, info.textureTopLeft.x, info.textureTopLeft.y, info.textureBtmRight.x, info.textureBtmRight.y, info.width, info.prekerning, info.postkerning);
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
	m_texture.Deinit();
}

bool beFont::LoadTexture(beRenderInterface* ri, const beWString& textureFilename)
{
	return m_texture.Init(ri, textureFilename);
}

static bool CompareTop32(const u64* _lhs, const u64* _rhs, int* comparison)
{
	u64 lhs = *_lhs >> 32;
	u64 rhs = *_rhs >> 32;
	if (lhs == rhs)
	{
		*comparison = 0;
		return true;
	}
	else if (lhs < rhs)
	{
		*comparison = -1;
		return false;
	}
	else
	{
		*comparison = 1;
		return false;
	}
}

const beFont::CharacterInfo* beFont::FindCharacterInfo(u32 c) const
{
	u64 searchKey = ((u64)c) << 32;
	const u64* entry = nullptr;
	if (m_characterIndices.BinarySearch<CompareTop32>(&searchKey, &entry))
	{
		return &m_characterInfo[(u32)*entry];
	}

	//int lowerBound = -1;
	//int upperBound = m_characterIndices.Count();
	//int index = upperBound / 2;
	//while (true)
	//{
	//	u64 entry = m_characterIndices[index];
	//	u64 entryKey = entry >> 32;
	//	if (entryKey == searchKey)
	//	{
	//		return &m_characterInfo[(u32)entry];
	//	}

	//	if (entry > searchKey)
	//	{
	//		upperBound = index;
	//	}
	//	else
	//	{
	//		lowerBound = index;
	//	}
	//	
	//	if (upperBound == lowerBound)
	//	{
	//		return nullptr;
	//	}
	//	int newIndex = (lowerBound + upperBound) / 2;
	//	if (index == newIndex)
	//	{
	//		newIndex++;
	//	}
	//	index = newIndex;
	//}

	return nullptr;
}

bool beFont::CompareExtraKerning(const beFont::ExtraKerning* _lhs, const beFont::ExtraKerning* _rhs, int* comparison)
{
	u64 lhs = _lhs->pair;
	u64 rhs = _rhs->pair;
	if (lhs == rhs)
	{
		*comparison = 0;
		return true;
	}
	else if (lhs < rhs)
	{
		*comparison = -1;
		return false;
	}
	else
	{
		*comparison = 1;
		return false;
	}
}

int beFont::GetKerning(u32 lhs, u32 rhs, const CharacterInfo* lastChar, const CharacterInfo* nextChar) const
{
	ExtraKerning searchKey;
	searchKey.pair = ((u64)lhs) << 32 | (u64)rhs;
	const ExtraKerning* extraKern = nullptr;
	if (m_extraKerning.BinarySearch<CompareExtraKerning>(&searchKey, &extraKern))
	{
		return extraKern->offset;
	}
	int kerning = nextChar->prekerning;
	if (lastChar)
	{
		kerning += lastChar->postkerning;
	}
	return kerning;
}

bool beFont::CreateString(beRenderInterface* ri, const beStringView& string, float scale, float maxWidth, u32 invalidStringCharacter, bool fixedWidth, WrapMode wrapMode, StringInfo* outStringInfo) const
{
	//float textureWidth = (float)m_texture->GetWidth();
	//float textureHeight = (float)m_texture->GetHeight();

	beWString wstring;
	beStringConversion::UTF8ToWide(string.c_str(), &wstring);

	const int vertsPerChar = 6;
	int numChars = (int)wstring.size();
	int numVerts = numChars * vertsPerChar;

	beVector<VertexInputType> vertices(numVerts, numVerts, 0);
	beVector<u32> indices(numVerts, numVerts, 0);

	for (int i = 0; i < numVerts; i++)
	{
		indices[i] = i;
	}

	const CharacterInfo* defaultCharacterInfo = FindCharacterInfo(invalidStringCharacter);
	const CharacterInfo* lastCharacterInfo = nullptr;
	int totalHeight = 0;
	float totalWidth = 0;
	int currentHeight = m_lineHeight;
	float lineWidth = 0;
	float wordWidth = 0;
	int wordChars = 0;
	u32 lastChar = 0;

	float fixedWidthValue = 0;
	if (fixedWidth)
	{
		for (const beFont::CharacterInfo& charInfo : m_characterInfo)
		{
			fixedWidthValue = beMath::Max(fixedWidthValue, (float)charInfo.width);
		}
		fixedWidthValue *= scale;
	}

	auto onNewLine = [&]() {
		currentHeight = totalHeight;
		lineWidth = 0;
		wordWidth = 0;
		wordChars = 0;
		lastChar = 0;
	};

	int vertIndex = 0;
	for (int i = 0; i < numChars; i++)
	{
		wchar_t nextChar = wstring[i];
		if (nextChar == '\n')
		{
			onNewLine();
			continue;
		}

		const CharacterInfo* charInfo = FindCharacterInfo(nextChar);
		if (!charInfo)
		{
			if (defaultCharacterInfo)
			{
				charInfo = defaultCharacterInfo;
			}
			else
			{
				continue;
			}
		}

		float kerning = (float)GetKerning(lastChar, nextChar, lastCharacterInfo, charInfo);
		float characterSize = (kerning + charInfo->width) * scale;
		float lineIncrement = fixedWidth ? fixedWidthValue : characterSize;
		float newLineWidth = lineWidth + lineIncrement;
		if (wrapMode != WrapMode::NoWrap && newLineWidth > maxWidth)
		{
			if (lineWidth == wordWidth)
			{
					// Only word, line break here.
				onNewLine();
				newLineWidth = charInfo->width * scale;
			}
			else if (wordChars != i)
			{
				// Move current word onto new line
				i -= (wordChars + 1);
				vertIndex -= vertsPerChar * wordChars;
				onNewLine();
				continue;
			}
			else
			{
				// Word does not fit on single line.
			}
		}

		if (nextChar == ' ')
		{
			wordWidth = 0;
			wordChars = 0;
		}
		else
		{
			wordChars++;
			wordWidth += charInfo->width * scale;
		}
		totalWidth = beMath::Max(totalWidth, newLineWidth);
		totalHeight = beMath::Max(totalHeight, currentHeight + m_lineHeight);



		float uvLeft = charInfo->textureTopLeft.x;
		float uvTop = charInfo->textureTopLeft.y;
		float uvRight = charInfo->textureBtmRight.x;
		float uvBottom = charInfo->textureBtmRight.y;
		float posLeft = lineWidth;// / textureWidth;
		float posRight = lineWidth + characterSize;// / textureWidth;
		float posTop = (float)(m_lineHeight - currentHeight) * scale;// / textureHeight;
		float posBtm = (float)(m_lineHeight - totalHeight) * scale;// / textureHeight;

		vertices[vertIndex+0].uv = Vec2(uvLeft, uvTop);
		vertices[vertIndex+4].uv = Vec2(uvLeft, uvBottom);
		vertices[vertIndex+2].uv = Vec2(uvRight, uvBottom);
		vertices[vertIndex+3].uv = Vec2(uvRight, uvBottom);
		vertices[vertIndex+1].uv = Vec2(uvRight, uvTop);
		vertices[vertIndex+5].uv = Vec2(uvLeft, uvTop);
		vertices[vertIndex+0].position = Vec2(posLeft, posTop);
		vertices[vertIndex+4].position = Vec2(posLeft, posBtm);
		vertices[vertIndex+2].position = Vec2(posRight, posBtm);
		vertices[vertIndex+3].position = Vec2(posRight, posBtm);
		vertices[vertIndex+1].position = Vec2(posRight, posTop);
		vertices[vertIndex+5].position = Vec2(posLeft, posTop);
		vertIndex += vertsPerChar;

		lineWidth = newLineWidth;
		lastChar = nextChar;
	}
	numVerts = vertIndex;

	outStringInfo->height = (float)totalHeight;
	outStringInfo->width = (float)totalWidth;

	bool success = outStringInfo->vertexBuffer.Allocate(ri, decltype(vertices)::element_size, numVerts, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, vertices.begin());
	if (!success) { BE_ASSERT(false); return false; }
	
	success = outStringInfo->indexBuffer.Allocate(ri, decltype(indices)::element_size, numVerts, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, indices.begin());
	if (!success) { BE_ASSERT(false); return false; }

	//for (int i = 0; i < numVerts; i++)
	//{
	//	bePRINTF("Pos: %f, %f, uv: %f, %f", vertices[i].position.x, vertices[i].position.y, vertices[i].uv.x, vertices[i].uv.y);
	//}

	return true;
}

const beTexture* beFont::GetTexture() const
{
	return &m_texture;
}
