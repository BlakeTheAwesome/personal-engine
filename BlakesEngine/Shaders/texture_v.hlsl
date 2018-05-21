#include "beShaderCBufferDefinitions.h"

CBUF_MatrixBuffer
CBUF_ColourBuffer

CS_TextureVertexInputType(VertexInputType)
CS_TexturePixelInputType(PixelInputType)

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	output.position = mul(input.position, worldToScreenMatrix);
	output.uv = input.uv;
	output.colour = colour;
	
	return output;
}

