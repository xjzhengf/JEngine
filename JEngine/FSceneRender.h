#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
#include "RenderItem.h"

class FSceneRender : public  FRenderResource{
public:
	std::vector<MeshData>& BuildMeshData();
	void BuildMaterial();
	static std::vector<MeshData> meshDataVector;
	RenderItem* mRenderItem;
	std::unordered_map<std::string, std::unique_ptr<FMaterial>> mMaterials;
};