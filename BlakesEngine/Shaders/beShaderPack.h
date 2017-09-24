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
};
