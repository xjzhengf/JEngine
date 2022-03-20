#pragma once
#include "stdafx.h"
#include "FRHIFactory.h"
#include "FRenderResource.h"
#include "FDynamicRHI.h"
class FRender {
public:
	bool Init();
	void RenderBegin();
	void Render(const GameTimer& gt);
	
	~FRender();
private:
	std::shared_ptr<FDynamicRHI> mRHI = nullptr;
	std::unique_ptr<FRenderResource> mRenderResource = nullptr;
	std::shared_ptr<FRHIResource> mRHIResource = nullptr;
	std::unique_ptr<FRHIFactory> RHIFactory = nullptr;
};