//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

#define Sample_RootSig \
"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )," \
"DescriptorTable(CBV(b0,numDescriptors = 1), visibility = SHADER_VISIBILITY_VERTEX),"\
"DescriptorTable(SRV(t0,numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL),"\
"RootConstants(b1, num32BitConstants = 3),"\
"StaticSampler(s0," \
                "addressU = TEXTURE_ADDRESS_WRAP," \
                "addressV = TEXTURE_ADDRESS_WRAP," \
                "addressW = TEXTURE_ADDRESS_WRAP," \
                "filter = FILTER_MIN_MAG_MIP_POINT)"

Texture2D    gDiffuseMap : register(t0);
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
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 TexC    : TEXCOORD;
};
[RootSignature(Sample_RootSig)]
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float3 POSL = vin.PosL;

	//POSL.z += sin(Time%180.0)*50;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(POSL, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
	vout.Color = vin.Color;
	vout.Normal = vin.Normal;
	//vout.TexC = vin.TexC; 
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), TexTransform);
	vout.TexC = texC.xy;
	return vout;
}

[RootSignature(Sample_RootSig)]
float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamPointWrap, pin.TexC);
	//float4 ResultColor = float4(pin.Normal*0.5f+0.5f,1.0f);

	return diffuseAlbedo;
}


