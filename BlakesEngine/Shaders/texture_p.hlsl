#include "beShaderCBufferDefinitions.h"

Texture2D shaderTexture;
SamplerState SampleType;

CS_TexturePixelInputType(PixelInputType)

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColour;
	
	textureColour = shaderTexture.Sample(SampleType, input.uv);
	
	textureColour *= input.colour;

	return textureColour;
}

