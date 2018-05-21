#include "beShaderCBufferDefinitions.h"

CBUF_MatrixBuffer
CBUF_CameraBuffer

CS_LightVertexInputType(VertexInputType)
CS_LightPixelInputType(PixelInputType)

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	output.position = mul(input.position, worldToScreenMatrix);

	output.normal = mul(input.normal, (float3x3)worldMatrix); // Calculate the normal vector against the world matrix only.
	output.normal = normalize(output.normal);
	output.tex = input.tex;

	float4 worldPosition = mul(input.position, worldMatrix);
	output.viewDirection = normalize(cameraPosition.xyz - worldPosition.xyz);
	
	return output;
}
