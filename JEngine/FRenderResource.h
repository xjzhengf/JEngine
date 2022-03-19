#pragma once
#include "stdafx.h"
class FRenderResource {
public:
	FRenderResource();
	FRenderResource(const FRenderResource& am) = delete;
	FRenderResource& operator=(const FRenderResource& am) = delete;
	~FRenderResource();
	virtual void ReleaseResource() {};
	std::vector<MeshData>& BuildMeshData();
public:
	static std::vector<MeshData> meshDataVector;
	static bool mRenderUpdate;

};