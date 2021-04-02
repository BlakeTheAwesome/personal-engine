#pragma once
#include "beShaderCBufferDefinitions.h"

#include "BlakesEngine/Core/beString.h"
import beMath;
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
	using VertexType = beShaderDefinitions::ShaderColour::VertexInputType;

	beShaderColour() = default;
	~beShaderColour();

	beShaderColour(const beShaderColour&) = delete;
	beShaderColour(beShaderColour&&) = delete;
	beShaderColour& operator=(const beShaderColour&) = delete;
	beShaderColour& operator=(beShaderColour&&) = delete;

	bool Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilename);
	void Deinit();

	void SetActive(beRenderInterface* renderInterface);
	void SetShaderParameters(beRenderInterface* renderInterface);
	
	void Render(beRenderInterface* renderInterface, int indexCount, int indexOffset);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	ID3D11PixelShader* m_pShader = nullptr;
	ID3D11VertexShader* m_vShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
};
