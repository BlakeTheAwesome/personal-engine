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
class beModel;
class beRenderInterface;

class beShaderLitTexture
{
public:
	using VertexType = beShaderDefinitions::ShaderLitTexture::VertexInputType;
	using ShaderParams = beShaderDefinitions::LightBuffer;
	

	beShaderLitTexture() = default;
	~beShaderLitTexture();

	beShaderLitTexture(const beShaderLitTexture&) = delete;
	beShaderLitTexture(beShaderLitTexture&&) = delete;
	beShaderLitTexture& operator=(const beShaderLitTexture&) = delete;
	beShaderLitTexture& operator=(beShaderLitTexture&&) = delete;

	bool Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename);
	void Deinit();

	static ShaderParams GetDefaultShaderParams(beRenderInterface* renderInterface);
	void SetShaderParameters(beRenderInterface* renderInterface, const ShaderParams& shaderParams);
	
	void Render(beRenderInterface* renderInterface, int indexCount, ID3D11ShaderResourceView* texture);
	void SetActive(beRenderInterface* renderInterface);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	ID3D11PixelShader* m_pShader = nullptr;
	ID3D11VertexShader* m_vShader = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	
	beRenderBuffer m_lightBuffer;
	beRenderBuffer m_cameraBuffer;

	ShaderParams m_lastShaderParams;
};
