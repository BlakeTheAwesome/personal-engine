#pragma once
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Math/beMath.h"
#include "BlakesEngine/Rendering/beRenderBuffer.h"

struct ID3D10Blob;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;

class beRenderInterface;

class beShaderColour
{
public:
	beShaderColour();
	~beShaderColour();

	bool Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename);
	void Deinit();

	void SetShaderParameters(beRenderInterface* renderInterface, const Matrix& viewMatrix);
	
	void Render(beRenderInterface* renderInterface, int indexCount, int indexOffset);
	void RenderMore(beRenderInterface* renderInterface, int indexCount, int indexOffset);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	ID3D11PixelShader* m_pShader;
	ID3D11VertexShader* m_vShader;
	
	ID3D11InputLayout* m_layout;
	beRenderBuffer m_matrixBuffer;
};
