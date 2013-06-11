#pragma once
#include "Core\beString.h"
#include "Math\beMath.h"

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
	
	void Render(beRenderInterface* renderInterface, int indexCount);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	ID3D11PixelShader* m_pShader;
	ID3D11VertexShader* m_vShader;
	
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};
