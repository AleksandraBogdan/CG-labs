//--------------------------------------------------------------------------------------
// File: Tutorial06.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vLightPos[2];
	float4 vLightColor[2];
	float4 vOutputColor;
	float radius;
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Norm : TEXCOORD0;
	float4 WPos : TEXCOORD1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.WPos = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Norm = mul(float4(input.Norm, 1), World).xyz;
	return output;
}

PS_INPUT VSVertex(VS_INPUT input)
{
	float3 finalColor = 0;
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	for (int i = 0; i < 2; i++)
	{
		float3 onPixel = (vLightPos[i] - output.Pos).xyz;
		if (length(onPixel) < radius) {
			onPixel = normalize(onPixel);
			finalColor += saturate(dot(onPixel, input.Norm) * vLightColor[i]).xyz;
		}
	}
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Norm = finalColor;
	return output;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalColor = 0;
	for (int i = 0; i < 2; i++)
	{
		float3 onPixel = (vLightPos[i] - input.WPos).xyz;
		if (length(onPixel) < radius)
		{
			onPixel = normalize(onPixel);
			finalColor += saturate(dot(onPixel, input.Norm) * vLightColor[i]);
		}
	}
	finalColor.a = 1;
	return finalColor;
}


float4 PSVertex(PS_INPUT input) : SV_Target
{
	float4 finalColor;
	finalColor.xyz = input.Norm;
    finalColor.a = 1.0;
	return finalColor;
}



//--------------------------------------------------------------------------------------
// PSSolid - render a solid color
//--------------------------------------------------------------------------------------
float4 PSSolid(PS_INPUT input) : SV_Target
{
	return vOutputColor;
}

