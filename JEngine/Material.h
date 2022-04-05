#pragma once
#include "stdafx.h"
#include "FPSO.h"
extern const int gNumFrameResources;
class FMaterial {
public:
	std::string Name;

	int MatCBIndex = -1;

	int DiffuseSrvHeapIndex = -1;

	int NormalSrvHeapIndex = -1;

	int NumFramesDirty = gNumFrameResources;
	PipelineState mPso ;
	std::wstring GlobalShader;
	void operator=(const FMaterial& m) {
		mPso = m.mPso;
		GlobalShader = m.GlobalShader;
	}
};

struct FMaterialConstants
{
	glm::vec4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;

	// Used in texture mapping.
	glm::mat4x4 MatTransform = glm::identity<glm::mat4x4>();
};