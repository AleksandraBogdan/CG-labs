//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
Texture2D tx1Diffuse : register(t1);

SamplerState samLinear : register( s0 );

cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
    float4 vMeshColor;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
	float2 Tex1 : TEXCOORD1;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float2 Tex1 : TEXCOORD1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
	output.Tex1 = input.Tex1;
    
    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalColor = 0;
	finalColor.a = 1;
	return finalColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_FirstTexture( PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex )* vMeshColor;
}

float4 PS_SecondTexture(PS_INPUT input) : SV_Target
{
	return tx1Diffuse.Sample(samLinear, input.Tex1)* vMeshColor;
}
float4 PS_SumTexture(PS_INPUT input) : SV_Target
{
	return (txDiffuse.Sample(samLinear, input.Tex) + tx1Diffuse.Sample(samLinear, input.Tex1))* 0.5* vMeshColor;
}
float4 PS_MulTexture(PS_INPUT input) : SV_Target
{
	return (txDiffuse.Sample(samLinear, input.Tex) * tx1Diffuse.Sample(samLinear, input.Tex1))* vMeshColor;
}