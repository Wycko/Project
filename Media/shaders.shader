cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

Texture2D shaderTexture;
SamplerState SampleType;


struct VOut
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};


struct POut
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


POut VShader( VOut input )
{
	POut output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul( input.position, worldMatrix );
	output.position = mul( output.position, viewMatrix );
	output.position = mul( output.position, projectionMatrix );

	output.tex = input.tex;

	return output;
}


float4 PShader( POut input ) : SV_TARGET
{
	float4 textureColor;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample( SampleType, input.tex );
	
	return textureColor;
}


float4 TPShader( POut input ) : SV_TARGET
{
	float4 textureColor;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample( SampleType, input.tex );

	float4 pixelColor;
	pixelColor.r = 1.0f;
	pixelColor.g = 0.0f;
	pixelColor.b = 1.0f;
	pixelColor.a = 1.0f;

	// If the color is black on the texture then treat this pixel as transparent.
	if( textureColor.r == 0.0f )
	{
		textureColor.a = 0.0f;
	}

	// If the color is other than black on the texture then this is a pixel in the font so draw it using the font pixel color.
	else
	{
		textureColor.a = 1.0f;
		textureColor = textureColor * pixelColor;
	}	

	return textureColor;
}