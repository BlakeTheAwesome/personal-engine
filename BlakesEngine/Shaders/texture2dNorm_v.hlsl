#include "beShaderCBufferDefinitions.h"

CBUF_OrthoMatrixBuffer
CBUF_PositionBuffer

CS_Texture2DVertexInputType(VertexInputType)
CS_TexturePixelInputType(PixelInputType)

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	float4 position = float4(input.position, 0.f, 1.f) + float4(positionOffset, 0.f, 0.f);
	//position = mul(position, orthoMatrixNormalised);

	output.position = position;
	output.uv = input.uv;
	output.colour = colour;
	
	return output;
}

