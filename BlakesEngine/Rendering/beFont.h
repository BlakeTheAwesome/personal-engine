#pragma once
#include "beRenderBuffer.h"
#include "beTexture.h"
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/DataStructures/beVector.h"
#include "BlakesEngine/Math/beMath.h"

class beRenderInterface;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beFont
{
public:

	struct VertexInputType
	{
		Vec2 position;
		Vec2 uv;
	};

	struct StringInfo
	{
		StringInfo() : width(0.f), height(0.f) {}
		beRenderBuffer vertexBuffer;
		beRenderBuffer indexBuffer;
		float width;
		float height;
	};

	beFont() = default;
	~beFont() = default;

	bool Init(beRenderInterface* ri, const char* filename, const beWString& textureFilename);
	void Deinit();

	// invalidStringCharacter will be used to replace unknown symbols, set to 0 to skip, or ' ' for a blank character
	bool CreateString(beRenderInterface* ri, const beStringView& string, float maxWidth, u32 invalidStringCharacter, bool fixedWidth, StringInfo* outStringInfo) const;
	
	const beTexture* GetTexture() const;

private:
	
	struct CharacterInfo
	{
		Vec2 textureTopLeft;
		Vec2 textureBtmRight;
		int width;
		int prekerning;
		int postkerning;
	};
	
	struct ExtraKerning // Special kerning pair
	{
		u64 pair; // top 32 bits for left character, bottom 32 for right character
		int offset;
	};

	bool ReadLine(const std::string & line);
	bool LoadTexture(beRenderInterface* ri, const beWString& textureFilename);
	const CharacterInfo* FindCharacterInfo(u32 c) const;
	static bool CompareExtraKerning(const beFont::ExtraKerning* lhs, const beFont::ExtraKerning* rhs, int* res);
	int GetKerning(u32 lhs, u32 rhs, const CharacterInfo* lastChar, const CharacterInfo* nextChar) const;

	beFixedVector<CharacterInfo, 128> m_characterInfo;
	beFixedVector<u64, 128> m_characterIndices; // Binary search for character in here. Top 32 bits are character code, bottom 32 are index into characterInfo.

	beFixedVector<ExtraKerning, 8> m_extraKerning; // Parse all then sortm, then binary search through this
	beTexture m_texture;
	int m_lineHeight = 0;
};
