#pragma once
#include "stdafx.h"
#include "FDynamicRHI.h"
#include "FRenderResource.h"
class FRender {
public:
	bool Init();
	void RenderBegin();
	void Render(const GameTimer& gt);
	
	~FRender();
private:
	std::unique_ptr<FDynamicRHI> mRHI = nullptr;
	std::unique_ptr<FRenderResource> mRenderResource = nullptr;
};