#include "beShaderCBufferDefinitions.h"

CBUF_MatrixBuffer

CS_ColourVertexInputType(VertexInputType)
CS_ColourPixelInputType(PixelInputType)

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	output.position = mul(input.position, worldToScreenMatrix);
	output.colour = input.colour;

	return output;
}
