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

class beShaderTexture2d
{
public:
	using VertexType = beShaderDefinitions::ShaderTexture2d::VertexInputType;

	enum class TextureMode
	{
		Wrapped,
		Clamped,
	};

	beShaderTexture2d() = default;
	~beShaderTexture2d();

	bool Init(beRenderInterface* renderInterface, const beWString& pixelFilename, const beWString& vertexFilenameNormalised, const beWString& vertexFilenamePixelSpace);
	void Deinit();

	void SetShaderParameters(beRenderInterface* renderInterface, const Matrix& viewMatrix);
	
	void RenderQuad(beRenderInterface* renderInterface, Vec2 uvMin, Vec2 uvMax, ID3D11ShaderResourceView* texture, bool normalisedVerts, TextureMode textureMode=TextureMode::Wrapped);
	void Render(beRenderInterface* renderInterface, int indexCount, ID3D11ShaderResourceView* texture, bool normalisedVerts, TextureMode textureMode=TextureMode::Wrapped);

	// Todo: add async loader
	bool IsLoaded() const;

private:
	ID3D11PixelShader* m_pShader = nullptr;
	ID3D11VertexShader* m_vShaderNormalised = nullptr;
	ID3D11VertexShader* m_vShaderPixelSpace = nullptr;
	ID3D11SamplerState* m_wrappedSampleState = nullptr;
	ID3D11SamplerState* m_clampedSampleState = nullptr;
	ID3D11InputLayout* m_layout = nullptr;

	// For RenderQuad
	beRenderBuffer m_vertexBuffer;
	beRenderBuffer m_indexBuffer;
	beRenderBuffer m_positionBuffer;
};
