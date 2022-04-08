#pragma once
#include "stdafx.h"
#include "FPSO.h"
extern const int gNumFrameResources;

struct FMaterialConstants
{
	glm::vec4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec3 FresnelR0 = { 0.6f, 0.6f, 0.6f };
	float Roughness = 0.3f;

	// Used in texture mapping.
	glm::mat4x4 MatTransform = glm::identity<glm::mat4x4>();
};
class FMaterial {
public:
	std::string Name;

	int MatCBIndex = -1;

	int DiffuseSrvHeapIndex = -1;

	int NormalSrvHeapIndex = -1;

	int NumFramesDirty = gNumFrameResources;
	PipelineState mPso ;
	std::wstring GlobalShader;	
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	void operator=(const FMaterial& m) {
		mPso = m.mPso;
		GlobalShader = m.GlobalShader;
	}
	FMaterialConstants mMaterialConstants;
};
