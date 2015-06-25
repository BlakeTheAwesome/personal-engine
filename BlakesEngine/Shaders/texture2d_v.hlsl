
// Set on the CPU
cbuffer MatrixBuffer
{
	matrix orthoMatrix;
	float2 screenSize;
	float2 padding;
};

cbuffer PositionBuffer
{
	float4 colour;
	float2 positionOffset;
	float2 padding2;
};

struct VertexInputType
{
	float2 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR0;
	float2 tex : TEXCOORD0;
};

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
	output.tex = input.tex;
	output.colour = colour;
	
	return output;
}
