//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
#include"RenderHead.hlsli"



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


