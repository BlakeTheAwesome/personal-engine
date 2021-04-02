#pragma once
#include "BlakesEngine/Core/beString.h"
import beMath;
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

class beShaderTexture
{
public:
	using VertexType = beShaderDefinitions::ShaderTexture::VertexInputType;

	beShaderTexture() = default;
	~beShaderTexture();

	beShaderTexture(const beShaderTexture&) = delete;
	beShaderTexture(beShaderTexture&&) = delete;
	beShaderTexture& operator=(const beShaderTexture&) = delete;
	beShaderTexture& operator=(beShaderTexture&&) = delete;

	bool Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename);
	void Deinit();

	void SetColour(const Vec4& colour);

	void SetActive(beRenderInterface* renderInterface);
	void SetShaderParameters(beRenderInterface* renderInterface);
	
	void Render(beRenderInterface* renderInterface, int indexCount, ID3D11ShaderResourceView* texture);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	ID3D11PixelShader* m_pShader = nullptr;
	ID3D11VertexShader* m_vShader = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	beRenderBuffer m_colourBuffer;

	Vec4 m_colour{1.f,1.f,1.f,1.f};
	bool m_colourDirty = true;
};
