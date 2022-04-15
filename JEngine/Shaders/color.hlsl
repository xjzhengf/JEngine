//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
#include"RenderHead.hlsli"

struct VertexIn
{
	float3 PosL  : POSITION;
	float2 TexC    : TEXCOORD;
	float2 UV : UV;

};
struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 TexC    : TEXCOORD;
	float2 UV : UV;
};

float Luminance(float3 InColor)
{
	return dot(InColor, float3(0.3f, 0.59f, 0.11f));
}

[RootSignature(Sample_RootSig)]
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH =float4( vin.PosL,1.0f);

	vout.TexC = vin.TexC;

	vout.UV = float2(1.0f,1.0f);

	return vout;
}

[RootSignature(Sample_RootSig)]
float4 PS(VertexOut pin) : SV_Target
{ 
	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];
	float2 DeltaUV = float2(DeltaU, DeltaV);
	float Width = RenderTargetSize[0] * 0.25f;
	float Height = RenderTargetSize[1] * 0.25f;
	int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);
    const float BloomThreshold = 1.0f;
    float StartRaduas = 2.0f / 14.0f;
	float2 Tex;
	Tex.x = 1.0f * X / Width;
	Tex.y = 1.0f * Y / Height;
	float4 Color0 = gBloomMap.Sample(gBloomInputSampler, Tex + float2(-DeltaUV.x, -DeltaUV.y));
	float4 Color1 = gBloomMap.Sample(gBloomInputSampler, Tex + float2(+DeltaUV.x, -DeltaUV.y));
	float4 Color2 = gBloomMap.Sample(gBloomInputSampler, Tex + float2(-DeltaUV.x, +DeltaUV.y));
	float4 Color3 = gBloomMap.Sample(gBloomInputSampler, Tex + float2(+DeltaUV.x, +DeltaUV.y));

	float4 AvailableColor = Color0 * 0.25f + Color1 * 0.25f + Color2 * 0.25f + Color3 * 0.25f;

	//AvailableColor.rgb = max(AvailableColor.rgb, 0) * OneOverPreExposure;

	float TotalLuminance = Luminance(AvailableColor.rgb);
	float BloomLuminance = TotalLuminance - BloomThreshold;
	float Amount = saturate(BloomLuminance * 0.5f);

	float4 OutColor;
	OutColor.rgb = AvailableColor.rgb;
	OutColor.rgb *= Amount;
	OutColor.a = 0.0f;

	return OutColor;
}

