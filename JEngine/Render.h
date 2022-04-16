#pragma once
#include "stdafx.h"
#include "FRHIFactory.h"
#include "FRenderResource.h"
#include "FRenderScene.h"
#include "FDynamicRHI.h"
class FRender {
public:
	bool Init();
	void Render();
	void RenderInit();
	void SceneRender();
	void DepthPass();
	void BloomPass(int index, const std::string& PSOName);
	void HDRPass();
	void ToneMapPass();
	void BuildLight(std::shared_ptr<FRenderScene> sceneResource);
	void BuildRenderItemTrans(std::shared_ptr<FRenderScene> sceneResource);
	~FRender();
private:
	glm::mat4x4 mWorld = glm::identity<glm::mat4x4>();
	std::shared_ptr<FDynamicRHI> mRHI = nullptr;
	std::shared_ptr<FRenderScene> mRenderScene = nullptr;
	std::shared_ptr<FRHIResource> mRHIResource = nullptr;
	std::shared_ptr<FRenderResource> mShadowResource = nullptr;
	std::shared_ptr<FRenderResource> mHDRResource = nullptr;
	std::unique_ptr<FRHIFactory> RHIFactory = nullptr;
};