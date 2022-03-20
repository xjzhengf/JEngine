#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
class FSceneRender : public  FRenderResource{
public:
	std::vector<MeshData>& BuildMeshData();
	static std::vector<MeshData> meshDataVector;
};