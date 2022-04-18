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

float3 ACESToneMapping(float3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
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
   float2 Tex;
	Tex.x = 1.0f * X / RenderTargetSize[0];
	Tex.y = 1.0f * Y / RenderTargetSize[1];

	float splitAmout = (1.0 + sin(Time * 6.0)) * 0.5;
	splitAmout *= 1.0 + sin(Time * 16.0) * 0.5;
	splitAmout *= 1.0 + sin(Time * 19.0) * 0.5;
	splitAmout *= 1.0 + sin(Time * 27.0) * 0.5;
	splitAmout = pow(splitAmout, 5);
	splitAmout *= (0.05 *1 );
	half3 finalColor;

	finalColor.g = gBloomMap.Sample(gBloomInputSampler, float2(Tex.x + splitAmout, Tex.y)).g;
	finalColor.r= gBloomMap.Sample(gBloomInputSampler, Tex).r;
	finalColor.b = gBloomMap.Sample(gBloomInputSampler, float2(Tex.x - splitAmout, Tex.y)).b;
	finalColor *= (1.0 - splitAmout * 0.5);

	float4 BloomColor = gBloomDown.Sample(gBloomInputSampler, Tex);

	half3 LinearColor = finalColor.rgb + BloomColor.rgb;
	
	float4 OutColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	OutColor.rgb = ACESToneMapping(LinearColor, 1.0f);
	

	return OutColor;
}

