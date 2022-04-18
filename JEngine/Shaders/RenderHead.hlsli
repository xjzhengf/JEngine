#define Sample_RootSig \
"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )," \
"RootConstants(b0, num32BitConstants = 8),"\
"DescriptorTable(CBV(b2,numDescriptors = 2), visibility = SHADER_VISIBILITY_ALL),"\
"DescriptorTable(SRV(t0,numDescriptors = 3), visibility = SHADER_VISIBILITY_PIXEL),"\
"DescriptorTable(SRV(t3,numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL),"\
"DescriptorTable(SRV(t4,numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL),"\
"DescriptorTable(SRV(t5,numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL),"\
"DescriptorTable(SRV(t6,numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL),"\
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
                "borderColor = STATIC_BORDER_COLOR_OPAQUE_BLACK),"\
"StaticSampler(s2," \
"addressU = TEXTURE_ADDRESS_WRAP," \
"addressV = TEXTURE_ADDRESS_WRAP," \
"addressW = TEXTURE_ADDRESS_WRAP," \
"filter = FILTER_MIN_MAG_MIP_LINEAR),"\
"StaticSampler(s3," \
"addressU = TEXTURE_ADDRESS_CLAMP," \
"addressV = TEXTURE_ADDRESS_CLAMP," \
"addressW = TEXTURE_ADDRESS_CLAMP," \
"filter = FILTER_MIN_MAG_LINEAR_MIP_POINT)"


Texture2D    gDiffuseMap : register(t0);
Texture2D    gNormalMap : register(t1);
Texture2D    gShadowMap : register(t2);
Texture2D    gBloomMap : register(t3);
Texture2D    gBloomDown : register(t4);
Texture2D    gBloomDown2 : register(t5);
Texture2D    gBloomUp : register(t6);
SamplerState gsamPointWrap        : register(s0);
SamplerComparisonState gSamShadow       : register(s1);
SamplerState gSamLinearWrap       : register(s2);
SamplerState gBloomInputSampler      : register(s3);
//SamplerState gsamAnisotropicWrap  : register(s4);
//SamplerState gsamAnisotropicClamp : register(s5);


int4 RenderTargetSize : register(b0);
float3 CameraLoc : register(b1);

struct Light {
	float Strength;
	float3 Direction;
	float4 Location;
};
struct Material {
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Roughness;
	float Shininess;
};

cbuffer cbPerObject : register(b2)
{
	float4x4 tLightViewProj;
	float4x4 gLightViewProj;
	float4x4 gViewProj;
	float4x4 gWorld;
	float4x4 gRotation;
	float4x4 TexTransform;
	float Time;
	Light light;
};
cbuffer materialConstants : register(b3)
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Roughness;
	float4x4 MatTransform;
};