#pragma once
#include "stdafx.h"
#include "Material.h"
#include "Buffer.h"
class RenderItem
{
public:
	RenderItem() {
		
	};
	glm::mat4x4 World = glm::identity<glm::mat4x4>();
	glm::mat4x4 Rotation = glm::identity<glm::mat4x4>();
	glm::mat4x4 MatTransform = glm::identity<glm::mat4x4>();

	int NumFramesDirty = gNumFrameResources;
	UINT ObjCBIndex = -1;
	UINT MaterialCBIndex = -1;
	UINT ObjSrvIndex = -1;
	FMaterial Mat;
	
	std::unique_ptr<DXBuffer> mGeo = nullptr;
	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};