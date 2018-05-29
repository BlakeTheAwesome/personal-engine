#pragma once
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Math/beMath.h"
#include "beRenderBuffer.h"
#include "beTexture.h"

class beRenderInterface;
class beShaderPack;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class beModel
{
public:
	struct LoadOptions
	{
		float scale = 1.f;
		bool flipFaces = false;
	};

	~beModel() { BE_ASSERT(!m_vertexBuffer.IsValid() && !m_indexBuffer.IsValid()); }

	bool Init(beRenderInterface* ri, beShaderPack* shaderPack, const beWString& textureFilename);
	bool InitWithFilename(beRenderInterface* ri, beShaderPack* shaderPack, const char* filename, const beWString& textureFilename, const LoadOptions& loadOptions);
	bool InitFromBuffers(beRenderBuffer* vertexBuffer, beRenderBuffer* indexBuffer);
	void Deinit();

	bool LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beWString& textureFilename);

	void Render(beRenderInterface* ri);
	
	ID3D11ShaderResourceView* GetTexture() const;
	const beRenderBuffer& GetVertexBuffer() const { return m_vertexBuffer; }
	const beRenderBuffer& GetIndexBuffer() const { return m_indexBuffer; }

	int GetIndexCount();

private:
	beTexture m_texture;
	beRenderBuffer m_vertexBuffer;
	beRenderBuffer m_indexBuffer;
};
