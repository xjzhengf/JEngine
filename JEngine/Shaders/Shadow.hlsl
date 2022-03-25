//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
#define Sample_RootSig \
"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )," \
"DescriptorTable(CBV(b0,numDescriptors = 1), visibility = SHADER_VISIBILITY_ALL),"\
"DescriptorTable(SRV(t0,numDescriptors = 3), visibility = SHADER_VISIBILITY_PIXEL),"\
"RootConstants(b1, num32BitConstants = 3),"\
"StaticSampler(s0," \
                "addressU = TEXTURE_ADDRESS_WRAP," \
                "addressV = TEXTURE_ADDRESS_WRAP," \
                "addressW = TEXTURE_ADDRESS_WRAP," \
                "filter = FILTER_MIN_MAG_MIP_POINT),"\
"StaticSampler(s1," \
                "addressU = TEXTURE_ADDRESS_BORDER," \
                "addressV = TEXTURE_ADDRESS_BORDER," \
                "addressW = TEXTURE_ADDRESS_BORDER," \
                "filter = FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,"\
                "mipLODBias =0 ,"\
                "maxAnisotropy = 16,"\
                "comparisonFunc = COMPARISON_LESS_EQUAL,"\
                "borderColor = STATIC_BORDER_COLOR_OPAQUE_BLACK)"

Texture2D    gDiffuseMap : register(t0);
Texture2D    gNormalMap : register(t1);
Texture2D    gShadowMap : register(t2);
SamplerState gsamPointWrap        : register(s0);
SamplerComparisonState gSamShadow       : register(s1);
//SamplerState gSamLinearWarp       : register(s2);
//SamplerState gsamLinearClamp      : register(s3);
//SamplerState gsamAnisotropicWrap  : register(s4);
//SamplerState gsamAnisotropicClamp : register(s5);


cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 tLightViewProj;
	float4x4 gLightViewProj;
	float4x4 gViewProj;
	float4x4 gWorld;
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
	float4x4 posW = mul(gWorld,gLightViewProj );
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(POSL, 1.0f), posW );
	return vout;
}

void PS(VertexOut pin) 
{
}


