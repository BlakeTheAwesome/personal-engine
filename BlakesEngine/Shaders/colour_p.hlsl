struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

float4 main(PixelInputType input) : SV_TARGET
{
	//float4 newPos = frac(input.position * 4);
	//float4 outPos = float4(newPos.x * input.color.x, newPos.y * input.color.y, newPos.z * input.color.z, 1);
	//float4 outPos = float4(input.color.x, input.color.y, input.color.z, 1);
	//return outPos;
	//input.colour.z = 0.5f;
	return input.colour;
}