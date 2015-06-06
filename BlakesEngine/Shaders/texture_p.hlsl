Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR0;
	float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColour;
	
	textureColour = shaderTexture.Sample(SampleType, input.tex);
	
	textureColour *= input.colour;

	return textureColour;
}

