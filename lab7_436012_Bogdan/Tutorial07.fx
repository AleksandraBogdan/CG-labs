//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
TextureCube txDiffuse : register(t0);
//Texture2D tx1Diffuse : register(t1);

SamplerState samLinear : register(s0);

/*cbuffer cbNeverChanges : register(b0)
{
	matrix View;
	float4 cameraPos;
};*/

cbuffer cbChangeOnResize : register(b1)
{
	matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b2)
{
	matrix World;
	matrix View;
	float4 cameraPos;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	//float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	// float2 Tex : TEXCOORD0;
	float3 R : REFLVECTOR;
	float3 Norm : TEXCOORD0;
	float FresnelCoef3 : INTCOEFF;
	float FresnelCoef8 : INTCOEFF1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	//output.Norm = normalize(input.Norm);
	output.Norm = normalize(mul(input.Norm, (float3x3)World));
	float3 I = normalize((float3)(mul(input.Pos, World) - cameraPos));
	output.R = normalize(reflect(I, output.Norm));
	float cosAlfa = -dot(I, output.Norm);
	output.FresnelCoef3 = 4 / pow((1 + cosAlfa), 3.5);
	output.FresnelCoef8 = 4 / pow((1 + cosAlfa), 8);
	if (output.FresnelCoef3 > 1)
		output.FresnelCoef3 = 1;
	if (output.FresnelCoef8 > 1)
		output.FresnelCoef8 = 1;

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalColor = 0;
	return finalColor;
}

float4 PS_Color(PS_INPUT input) : SV_Target
{
	float4 finalColor = 0;
	finalColor.g = 1;
	finalColor.b = 1;
	finalColor.a = 1;
	//float3 Norm = normalize(input.Norm);
	return finalColor;
}

float4 PS_CubeMap(PS_INPUT input) : SV_Target
{

	//float3 R = normalize(reflect(input.I, input.Norm));
	return saturate(txDiffuse.Sample(samLinear,input.R));
}

float4 PS_Fresnel(PS_INPUT input) : SV_Target
{
	float4 finalColor = 0;
	finalColor.g = 1;
	finalColor.b = 1;
	finalColor.a = 1;
	//float3 R = normalize(reflect(input.I, input.Norm));
	//float cosAlfa = dot(input.I,input.Norm) ;
	//float coef = 1.0 / pow((1.0 + cosAlfa), 3.5);
	//if (coef > 1)
		//coef = 1;
	return saturate(txDiffuse.Sample(samLinear, input.R) * (input.FresnelCoef3)  + finalColor * (1 - input.FresnelCoef3));
	//return float4(input.FresnelCoef, 0.0, 0.0, 0.0);
//	return saturate(txDiffuse.Sample(samLinear, R) * input.FresnelCoef + finalColor * (1 - input.FresnelCoef));
	//return saturate(txDiffuse.Sample(samLinear, R) * (coef) + finalColor * (1 -  coef));
}

float4 PS_Fresnel8(PS_INPUT input) : SV_Target
{
	float4 finalColor = 0;
	finalColor.g = 1;
	finalColor.b = 1;
	finalColor.a = 1;
	
	return saturate(txDiffuse.Sample(samLinear, input.R) * (input.FresnelCoef8) + finalColor * (1 - input.FresnelCoef8));
	
}

