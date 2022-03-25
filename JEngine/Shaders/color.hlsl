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

struct Light {
	float Brightness;
	float3 Direction;
	float4 Location;
};

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
	Light light;
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
	float4 ShadowPosH : POSITION0;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 TexC    : TEXCOORD;
};
[RootSignature(Sample_RootSig)]
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float3 POSL = vin.PosL;
	//POSL.z += sin(Time)*100;
	float4 posW = mul(float4(POSL, 1.0f), gWorld);
	
	vout.PosH = mul(posW, gViewProj);
	vout.ShadowPosH = mul(posW, tLightViewProj);
	vout.Color = vin.Color;
	vout.Normal = vin.Normal;

	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), TexTransform);
	vout.TexC = texC.xy;
	


	return vout;
}




//float4 ComputeLighting(float3 shadowFactor,Light gLights,float3 toEye){
//	float3 result = 0.0f;
//    int i = 0;
//
//    for(i = 0; i < 3; ++i)
//    {
//        result += shadowFactor[i] * ComputeDirectionalLight(gLights, toEye);
//    }
//
//}
//float3 ComputeDirectionalLight(Light L, float3 normal, float3 toEye){
//
//}



float CalcShadowFactor(float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float currentDepth = shadowPosH.z;

	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);

	// Texel size.
	float dx = 1.0f / (float)width;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += gShadowMap.SampleCmpLevelZero(gSamShadow, shadowPosH.xy + offsets[i], currentDepth).r;
	}

	return percentLit / 9.0f;
}

float ShadowCalculation(float4 shadowPosH) {
	
	shadowPosH.xyz /= shadowPosH.w;
	float depth = shadowPosH.z;
	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);
	float2 PiexlPos;
	PiexlPos= shadowPosH.xy* width;

	float depthInMap = gShadowMap.Load(int3(PiexlPos, 0)).r;
	return depth > depthInMap ? 0 : 1;

}
[RootSignature(Sample_RootSig)]
float4 PS(VertexOut pin) : SV_Target
{

	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamPointWrap, pin.TexC)* gNormalMap.Sample(gsamPointWrap, pin.TexC);

#ifdef ALPHA_TEST
	clip(diffuseAlbedo.a - 0.1f);
#endif
	//float4 ambient = float4(0.25f, 0.25f, 0.35f, 1.0f) * diffuseAlbedo;
	float shadowFactor = CalcShadowFactor(pin.ShadowPosH);
	//float shadowFactor = ShadowCalculation(pin.ShadowPosH);
	//if (shadowFactor == 0) {
	//	diffuseAlbedo = ambient;
	//}
	return diffuseAlbedo * (shadowFactor + 0.1);
}

