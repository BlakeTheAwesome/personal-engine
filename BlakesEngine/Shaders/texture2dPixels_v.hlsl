#include "beShaderCBufferDefinitions.h"

CBUF_OrthoMatrixBuffer
CBUF_PositionBuffer

CS_Texture2DVertexInputType(VertexInputType)
CS_TexturePixelInputType(PixelInputType)

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	float2 pos2 = input.position + positionOffset - (screenSize/2);
	float4 position = float4(pos2, 0.f, 1.f);

	//position = mul(position, orthoMatrixPixels);
	position = mul(position, orthoMatrixNormalised);
	

	output.position = position;
	output.uv = input.uv;
	output.colour = colour;
	
	return output;
}

