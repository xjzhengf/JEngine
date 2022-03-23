//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

#define Sample_RootSig \
"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )," \
"DescriptorTable(CBV(b0,numDescriptors = 1), visibility = SHADER_VISIBILITY_ALL),"\
"DescriptorTable(SRV(t0,numDescriptors = 2), visibility = SHADER_VISIBILITY_PIXEL),"\
"RootConstants(b1, num32BitConstants = 3),"\
"StaticSampler(s0," \
                "addressU = TEXTURE_ADDRESS_WRAP," \
                "addressV = TEXTURE_ADDRESS_WRAP," \
                "addressW = TEXTURE_ADDRESS_WRAP," \
                "filter = FILTER_MIN_MAG_MIP_POINT)"

Texture2D    gDiffuseMap : register(t0);
Texture2D    gNormalMap : register(t1);
SamplerState gsamPointWrap        : register(s0);
//SamplerState gsamPointClamp       : register(s1);
//SamplerState gsamLinearWrap       : register(s2);
//SamplerState gsamLinearClamp      : register(s3);
//SamplerState gsamAnisotropicWrap  : register(s4);
//SamplerState gsamAnisotropicClamp : register(s5);


cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 Rotation;
	float4x4 Scale;
	float4x4 Translate;
	float4x4 TexTransform;
	float Time;
};
float3 CameraLoc : register(b1);

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

	vout.PosH = mul(float4(POSL, 1.0f), gWorldViewProj);
	return vout;
}

void PS(VertexOut pin) 
{

}

