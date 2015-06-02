
// Set on the CPU
cbuffer MatrixBuffer
{
	//matrix worldMatrix;
	//matrix viewMatrix;
	matrix orthoMatrix;
};

cbuffer PositionBuffer
{
	float2 positionOffset;
	float2 padding;
};

struct VertexInputType
{
	float2 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType Texture2dVertexShader(VertexInputType input)
{
	PixelInputType output;

	float4 position = float4(input.position + positionOffset, 0.f, 1.f);
	//float4 worldPosition = mul(input.position, worldMatrix); // CPU should be sending us w=1 on our position, if not our matrix will misbehave.

	// Calculate the position of the vertex against the world, view, and projection matrices.
	//output.position = worldPosition;
	//output.position = mul(output.position, viewMatrix);
	output.position = mul(position, orthoMatrix);

	output.tex = input.tex;
	
	return output;
}

