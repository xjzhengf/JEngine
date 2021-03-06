#include"RenderHead.hlsli"


struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentU : TANGENT;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 ShadowPosH : POSITION0;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentW : TANGENT;
};


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
	PiexlPos = shadowPosH.xy * width;

	float depthInMap = gShadowMap.Load(int3(PiexlPos, 0)).r;
	return depth > depthInMap ? 0 : 1;

}

float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	const float m = mat.Shininess * 256.0f;
	float3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal) * 0.5 + 0.5, 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

	float3 specAlbedo = fresnelFactor * roughnessFactor;

	// Our spec formula goes outside [0,1] range, but we are 
	// doing LDR rendering.  So scale it down a bit.
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);

	return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = L.Direction;

	// Scale light down by Lambert's cosine law.
	float ndotl = max(dot(lightVec, normal) * 0.5 + 0.5, 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}
float4 ComputeLighting(Light gLights, Material mat,
	float3 pos, float3 normal, float3 toEye,
	float shadowFactor)
{
	float3 result = 0.0f;
	result = shadowFactor * ComputeDirectionalLight(gLights, mat, normal, toEye);
	return float4(result, 0.0f);
}
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	float3 normalT = 2.0f * normalMapSample - 1.0f;

	float3 N = normalize(unitNormalW);
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = normalize(cross(N, T));

	float3x3 TBN = float3x3(T, B, N);

	float3 bumpedNormalW = mul(normalT, TBN);
	return bumpedNormalW;
}


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
	vout.Normal = mul(vin.Normal.xyz, (float3x3)gWorld);
	vout.TangentW = mul(vin.TangentU.xyz, (float3x3)gWorld);
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), TexTransform);
	vout.TexC = texC.xy;
	return vout;
}

[RootSignature(Sample_RootSig)]
float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = DiffuseAlbedo;
	float3 fresnelR0 = FresnelR0;
	float  roughness = Roughness;
	diffuseAlbedo *= gDiffuseMap.Sample(gsamPointWrap, pin.TexC);

#ifdef ALPHA_TEST
	clip(diffuseAlbedo.a - 0.1f);
#endif
	pin.TangentW = normalize(pin.TangentW);
	pin.Normal = normalize(pin.Normal);
	float4 normalMap = gNormalMap.Sample(gsamPointWrap, pin.TexC);
	float3 bumpedNormalW;
	if (normalMap.r == 0 && normalMap.g == 0 && normalMap.b == 0) {
		bumpedNormalW = NormalSampleToWorldSpace(normalMap.rgb, pin.Normal, pin.TangentW);
	}
	else {
		bumpedNormalW = pin.Normal;
	}
	float4 gAmbientLight = diffuseAlbedo * 0.1;
	float4 ambient = gAmbientLight * diffuseAlbedo;
	float3 toEyeW = normalize(CameraLoc - pin.PosH);
	float shadowFactor = CalcShadowFactor(pin.ShadowPosH);
	const float shininess = (1.0f - roughness) * normalMap.a;
	Material mat = { diffuseAlbedo, fresnelR0, roughness ,shininess };
	float4 directLight = ComputeLighting(light, mat, pin.PosH,
		bumpedNormalW, toEyeW, shadowFactor);
	float4 litColor = ambient + directLight;
	return litColor;
}

