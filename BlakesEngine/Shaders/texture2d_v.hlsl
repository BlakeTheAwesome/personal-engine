#include "beShaderCBufferDefinitions.h"

CBUF_OrthoMatrixBuffer
CBUF_PositionBuffer

CS_Texture2DVertexInputType(VertexInputType)
CS_TexturePixelInputType(PixelInputType)

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	float4 position = float4(input.position, 0.f, 1.f);
	position = mul(position, orthoMatrix);
	position = position + float4(positionOffset, 0.f, 0.f) - float4(1.f, 1.f, 0.f, 0.f);

	output.position = position;
	output.uv = input.uv;
	output.colour = colour;
	
	return output;
}

