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

float2 Circle(float Start, float Points, float Point)
{
	float Radians = (2.0f * 3.141592f * (1.0f / Points)) * (Start + Point);
	return float2(cos(Radians), sin(Radians));
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
    int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);
	float4 OutColor = float4(0.0f, 0.0f, 0.0f, 0.0f);


	float BloomWeightScalar = 1.0f / 5.0f;
	float BloomWeightScalar1 = 1.0f / 5.0f;

	float BloomUpScale = 1.32f;

	float2 Tex;
	Tex.x = 1.0f * X / RenderTargetSize[2];
	Tex.y = 1.0f * Y / RenderTargetSize[3];

	float DeltaU = 1.0f / RenderTargetSize[2];
	float DeltaV = 1.0f / RenderTargetSize[3];
	float2 DeltaUV = float2(DeltaU, DeltaV);

	float Start = 2.0 / 7.0;
	float4 Color0 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 0.0f));
	float4 Color1 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 1.0f));
	float4 Color2 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 2.0f));
	float4 Color3 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 3.0f));
	float4 Color4 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 4.0f));
	float4 Color5 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 5.0f));
	float4 Color6 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 6.0f));
	float4 Color7 = gBloomUp.Sample(gBloomInputSampler, Tex);

	float4 Color8 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 0.0f));
	float4 Color9 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 1.0f));
	float4 Color10 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 2.0f));
	float4 Color11 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 3.0f));
	float4 Color12 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 4.0f));
	float4 Color13 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 5.0f));
	float4 Color14 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 6.0f));
	float4 Color15 = gBloomMap2.Sample(gBloomInputSampler, Tex);

	float4 BloomWight = float4(BloomWeightScalar, BloomWeightScalar, BloomWeightScalar, 0.0f);
	float4 BloomWight1 = float4(BloomWeightScalar1, BloomWeightScalar1, BloomWeightScalar1, 0.0f);

	OutColor = (Color0 + Color1 + Color2 + Color3 + Color4 + Color5 + Color6 + Color7) * BloomWight +
		(Color8 + Color9 + Color10 + Color11 + Color12 + Color13 + Color14 + Color15) * BloomWight1;
	OutColor.a = 0.0f;


	return OutColor;
}

