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
	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];
	float2 DeltaUV = float2(DeltaU, DeltaV);
	float Width = RenderTargetSize[2] ;
	float Height = RenderTargetSize[3];
	int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);
	float BloomDownScale = 2.5f;
	float2 Tex;
	Tex.x = 1.0f * X / Width;
	Tex.y = 1.0f * Y / Height;
	float StartRaduas = 2.0f / 14.0f;
	float4 Color0 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 0.0f));
	float4 Color1 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 1.0f));
	float4 Color2 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 2.0f));
	float4 Color3 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 3.0f));
	float4 Color4 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 4.0f));
	float4 Color5 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 5.0f));
	float4 Color6 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 6.0f));
	float4 Color7 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 7.0f));
	float4 Color8 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 8.0f));
	float4 Color9 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 9.0f));
	float4 Color10 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 10.0f));
	float4 Color11 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 11.0f));
	float4 Color12 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 12.0f));
	float4 Color13 = gBloomMap2.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 13.0f));

	float4 Color = gBloomMap2.Sample(gBloomInputSampler, Tex);

	float Weight = 1.0f / 15.0f;

	Color = Weight * (Color + Color0 + Color1 + Color2 + Color3
		+ Color4 + Color5 + Color6 + Color7
		+ Color8 + Color9 + Color10 + Color11 + Color12 + Color13);

	return Color;
}

