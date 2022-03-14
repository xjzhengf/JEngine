//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float4x4 Rotation;
	float4x4 Scale;
	float4x4 Translate;
	float Time;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float3 Normal : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float3 POSL = vin.PosL;

	POSL.z += sin(Time%180.0)*50;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(POSL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    vout.Normal = vin.Normal;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 ResultColor = float4(pin.Normal*0.5f+0.5f,1.0f);
    return ResultColor;
}


