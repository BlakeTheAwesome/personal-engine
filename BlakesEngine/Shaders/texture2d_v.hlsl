#include "beShaderCBufferDefinitions.h"

CBUF_OrthoMatrixBuffer
CBUF_PositionBuffer

CS_Texture2DVertexInputType(VertexInputType)
CS_TexturePixelInputType(PixelInputType)

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	float2 realPosition = (input.position + positionOffset);
	float4 position = float4(realPosition, 0.f, 1.f);
	//float4 worldPosition = mul(input.position, worldMatrix); // CPU should be sending us w=1 on our position, if not our matrix will misbehave.

	// Calculate the position of the vertex against the world, view, and projection matrices.
	//output.position = worldPosition;
	//output.position = mul(output.position, viewMatrix);
	position = mul(position, orthoMatrix);

	output.position = position;
	output.uv = input.uv;
	output.colour = colour;
	
	return output;
}

