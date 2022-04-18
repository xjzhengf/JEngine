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

float2 SunShaftPosToUV(float2 Pos)
{

	return Pos.xy * float2(0.5, -0.5) + 0.5;
}

float2 SunPos()
{
	float4 LightShaftCenter = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return LightShaftCenter.xy;
}

float2 SunShaftRect(float2 InPosition, float amount)
{
	float2 center = SunPos();
	return SunShaftPosToUV(lerp(center, InPosition, amount));
}

float2 VignetteSpace(float2 Pos, float AspectRatio)
{

	float Scale = sqrt(2.0) / sqrt(1.0 + AspectRatio * AspectRatio);
	return Pos * float2(1.0, AspectRatio) * Scale;
}

float Square(float x)
{
	return x * x;
}

float ComputeVignetteMask(float2 VignetteCircleSpacePos, float Intensity)
{
	VignetteCircleSpacePos *= Intensity;
	float Tan2Angle = dot(VignetteCircleSpacePos, VignetteCircleSpacePos);
	float Cos4Angle = Square(rcp(Tan2Angle + 1));
	return Cos4Angle;
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

	//float4 BloomColor = gBloomDown.Sample(gBloomInputSampler, Tex);

	//half3 LinearColor = finalColor.rgb + BloomColor.rgb;
	
	float4 OutColor = float4(finalColor, 0.0f);
	return OutColor;
}

