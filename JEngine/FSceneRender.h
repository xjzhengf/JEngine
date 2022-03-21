#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
#include "RenderItem.h"
class FSceneRender : public  FRenderResource{
public:
	std::vector<MeshData>& BuildMeshData();
	static std::vector<MeshData> meshDataVector;
	std::vector<RenderItem>& BuildRenderItem();
};