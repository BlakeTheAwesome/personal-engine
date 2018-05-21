#pragma once
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Math/beMath.h"
#include "BlakesEngine/Rendering/beRenderBuffer.h"
#include "beShaderCBufferDefinitions.h"

struct ID3D10Blob;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

class beRenderInterface;

class beShaderLitTexture
{
public:
	using VertexType = beShaderDefinitions::ShaderLitTexture::VertexInputType;

	beShaderLitTexture() = default;
	~beShaderLitTexture();

	bool Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename);
	void Deinit();

	void SetShaderParameters(beRenderInterface* renderInterface, const Matrix& viewMatrix);
	
	void Render(beRenderInterface* renderInterface, int indexCount, ID3D11ShaderResourceView* texture);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	ID3D11PixelShader* m_pShader = nullptr;
	ID3D11VertexShader* m_vShader = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	
	beRenderBuffer m_lightBuffer;
	beRenderBuffer m_cameraBuffer;
};
