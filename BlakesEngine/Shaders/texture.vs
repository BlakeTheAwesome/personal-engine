
// Set on the CPU
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	float4 worldPosition = mul(input.position, worldMatrix); // CPU should be sending us w=1 on our position, if not our matrix will misbehave.

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = worldPosition;
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;
	
	return output;
}

