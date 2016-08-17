#pragma once
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Math/beMath.h"
#include "BlakesEngine/Rendering/beRenderBuffer.h"

struct ID3D10Blob;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

class beRenderInterface;

class beShaderTexture
{
public:
	beShaderTexture();
	~beShaderTexture();

	bool Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename);
	void Deinit();

	void SetColour(const Vec4& colour);

	void SetShaderParameters(beRenderInterface* renderInterface, const Matrix& viewMatrix);
	
	void Render(beRenderInterface* renderInterface, int indexCount, ID3D11ShaderResourceView* texture);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	ID3D11PixelShader* m_pShader;
	ID3D11VertexShader* m_vShader;
	ID3D11SamplerState* m_sampleState;

	ID3D11InputLayout* m_layout;
	beRenderBuffer m_matrixBuffer;
	beRenderBuffer m_colourBuffer;

	Vec4 m_colour;
	bool m_colourDirty;
};
