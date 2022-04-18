//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
#include"RenderHead.hlsli"


//cbuffer cbPerObject : register(b2)
//{
//	float4x4 tLightViewProj;
//	float4x4 gLightViewProj;
//	float4x4 gViewProj;
//	float4x4 gWorld;
//	float4x4 gRotation;
//	float4x4 TexTransform;
//	float Time;
//};
//cbuffer materialConstants : register(b3)
//{
//	float4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
//	float3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
//	float Roughness = 0.25f;
//	float4x4 MatTransform ;
//};

struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};
[RootSignature(Sample_RootSig)]
VertexOut VS(VertexIn vin)
{

	VertexOut vout;
	float3 POSL = vin.PosL;
	float4x4 posW = mul(gWorld,gLightViewProj );
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(POSL, 1.0f), posW );
	return vout;
}

void PS(VertexOut pin) 
{
	float4 test = DiffuseAlbedo;
}


