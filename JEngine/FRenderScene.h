#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
#include "RenderItem.h"

class FRenderScene : public  FRenderResource{
public:
	std::unordered_map<std::string, MeshData>& BuildMeshData();
	void BuildMaterial();
	static std::unordered_map<std::string,MeshData> meshDataVector;
	std::map <std::string, std::unique_ptr<RenderItem>> mRenderItem;
	std::unordered_map<std::string, std::unique_ptr<FMaterial>> mMaterials;
	glm::mat4x4 LightViewProj;
	glm::mat4x4 TLightViewProj;
};