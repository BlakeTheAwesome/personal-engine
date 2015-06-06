#pragma once
#include "Core\beString.h"
#include "DataStructures/beVector.h"
#include "Math\beMath.h"

class beRenderInterface;
class beTexture;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beFont
{
public:
	struct StringInfo
	{
		float width;
		float height;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
	};

	beFont();
	~beFont();

	bool Init(beRenderInterface* ri, const char* filename, const beWString& textureFilename);
	void Deinit();

	// invalidStringCharacter will be used to replace unknown symbols, set to 0 to skip, or ' ' for a blank character
	bool CreateString(float maxWidth, u32 invalidStringCharacter, StringInfo* outStringInfo);
	
	ID3D11ShaderResourceView* GetTexture() const;

private:
	bool ReadLine(const std::string & line);
	bool LoadTexture(beRenderInterface* ri, const beWString& textureFilename);
	
	struct CharacterInfo
	{
		Vec2 textureTopLeft;
		Vec2 textureBottomRight;
		int width;
		int prekerning;
		int postkerning;
	};
	
	struct ExtraKerning // Special kerning pair
	{
		u64 pair; // top 32 bits for left character, bottom 32 for right character
		int offset;
	};

	beVector<CharacterInfo> m_characterInfo;
	beVector<u64> m_characterIndices; // Binary search for character in here. Top 32 bits are character code, bottom 32 are index into characterInfo.

	beVector<ExtraKerning> m_extraKerning; // Parse all then sortm, then binary search through this
	beTexture* m_texture;
};
