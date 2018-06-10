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
		enum {swizX=0, swizY=1, swizZ=2};

		beRendering::Topology topology = beRendering::Topology::TriangleList;
		bool flipFaces = false;
		float scale = 1.f;
		beArray<int, 3> axesSwizzle ={swizX, swizY, swizZ};
	};

	~beModel() { BE_ASSERT(!m_vertexBuffer.IsValid() && !m_indexBuffer.IsValid()); }

	bool Init(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename);
	bool InitWithFilename(beRenderInterface* ri, beShaderPack* shaderPack, const char* filename, const beStringView& textureFilename, const LoadOptions& loadOptions);
	bool InitFromBuffers(beRenderBuffer* vertexBuffer, beRenderBuffer* indexBuffer);
	void Deinit();

	bool LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename);

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
