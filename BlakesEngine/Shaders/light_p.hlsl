Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 ambientColour;
    float4 diffuseColour;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float3 viewDirection : TEXCOORD1;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColour;
    float3 lightDir;
    float lightIntensity;
    float4 colour;
	float4 specular = {0.f, 0.f, 0.f, 0.f};

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColour = shaderTexture.Sample(SampleType, input.tex);

	//Invert the light direction for calculations.
	lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, lightDir));

	colour = ambientColour;

	if (lightIntensity > 0.f)
	{
		colour += diffuseColour * lightIntensity;

		float3 reflectionVector = normalize(2 * lightIntensity * input.normal - lightDirection);
		specular = specularColor * pow(saturate(dot(input.viewDirection, reflectionVector)), specularPower);
	}

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	colour = saturate(colour) * textureColour;
	colour = saturate(colour + specular);
	
	return colour;
}