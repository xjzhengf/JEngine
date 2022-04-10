#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
#include "RenderItem.h"
#include "FTexture.h"
class FRenderScene {
public:
	std::unordered_map<std::string, MeshData>& BuildMeshData();
	static std::unordered_map<std::string,MeshData> meshDataVector;
	std::map <std::string, std::shared_ptr<RenderItem>> mRenderItem;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mNormalTextures;
	glm::mat4x4 LightViewProj;
	glm::mat4x4 TLightViewProj;
	glm::vec3 LightDirection;
};