#pragma once
#include "beShaderColour.h"
#include "beShaderLitTexture.h"
#include "beShaderTexture.h"
#include "beShaderTexture2d.h"

struct beShaderPack
{
	beShaderColour shaderColour;
	beShaderLitTexture shaderLitTexture;
	beShaderTexture shaderTexture;
	beShaderTexture2d shaderTexture2d;

	beShaderPack();
	~beShaderPack();
	void Init(beRenderInterface* ri);
	void Deinit();
	void UpdateFrameBuffers(beRenderInterface* ri, const Matrix& viewMatrix);
	private:
	struct FrameData;
	std::unique_ptr<FrameData> m_frameData;
};
