module;
#include "BlakesEngine/Core/beString.h"
#include <d3d11.h>
#include <optional>
export module beTexture;

import beTypes;
import beNonCopiable;
import beMath;
import beShaderPack;
import beRenderInterface;


export enum class beTextureFormat : u8
{
	Invalid,
	R8G8B8A8_UNORM,
	R32G32B32_FLOAT,
};

export class beTexture : NonCopiable
{
public:
	
	struct LoadOptions
	{
		int height = 0;
		int width = 0;
		int mipLevels = 0;
		beTextureFormat format = beTextureFormat::Invalid;
		beString additionalLoadDir;
		bool cpuReadable = false;
		bool cpuWritable = false;
	};

	beTexture() = default;
	~beTexture();
	beTexture(beTexture&&) noexcept;
	beTexture& operator=(beTexture&&) noexcept;

	bool Init(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename, std::optional<std::reference_wrapper<LoadOptions>> loadOptions);
	bool InitAsTarget(beRenderInterface* ri, const LoadOptions& loadOptions);
	void FinaliseTarget();
	void Deinit();

	void Set(const beTexture& that); // inc refs.

	int GetWidth() const;
	int GetHeight() const;

	ID3D11ShaderResourceView* GetTexture() const;

	void SetAsTarget(beRenderInterface* ri);
	void Clear(beRenderInterface* ri, Vec4 clearColour);

private:
	beTexture(const beTexture&) = delete;
	beTexture& operator=(const beTexture&) = default;

	D3D11_TEXTURE2D_DESC m_desc{};
	mutable ID3D11ShaderResourceView* m_texture = nullptr;
	ID3D11Texture2D* m_texture2d = nullptr;

	// When writable
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
};
