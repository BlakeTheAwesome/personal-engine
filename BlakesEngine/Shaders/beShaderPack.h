#pragma once
#include "beShaderColour.h"
#include "beShaderLitTexture.h"
#include "beShaderTexture.h"
#include "beShaderTexture2d.h"

class beShaderPack
{
	public:

	beShaderPack();
	~beShaderPack();

	beShaderPack(const beShaderPack&) = delete;
	beShaderPack(beShaderPack&&) = delete;
	beShaderPack& operator=(const beShaderPack&) = delete;
	beShaderPack& operator=(beShaderPack&&) = delete;

	void Init(beRenderInterface* ri);
	void Deinit();
	void UpdateFrameBuffers(beRenderInterface* ri, const Matrix& viewMatrix);

	beShaderColour shaderColour;
	beShaderLitTexture shaderLitTexture;
	beShaderTexture shaderTexture;
	beShaderTexture2d shaderTexture2d;

	private:
	struct FrameData;
	std::unique_ptr<FrameData> m_frameData;
};
