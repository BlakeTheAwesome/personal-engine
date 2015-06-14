#pragma once
#include "BlakesEngine\Core\beString.h"
#include "BlakesEngine\Math\beMath.h"

struct ID3D10Blob;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

class beRenderInterface;

class beShaderTexture2d
{
public:
	enum class TextureMode
	{
		Wrapped,
		Clamped,
	};

	beShaderTexture2d();
	~beShaderTexture2d();

	bool Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename);
	void Deinit();

	void SetShaderParameters(beRenderInterface* renderInterface, const Matrix& viewMatrix);
	
	void Render(beRenderInterface* renderInterface, int indexCount, ID3D11ShaderResourceView* texture, TextureMode textureMode=TextureMode::Wrapped);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	ID3D11PixelShader* m_pShader;
	ID3D11VertexShader* m_vShader;
	ID3D11SamplerState* m_wrappedSampleState;
	ID3D11SamplerState* m_clampedSampleState;

	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};
