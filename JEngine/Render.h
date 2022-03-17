#pragma once
#include "stdafx.h"
#include "RHI.h"
class FRender {
public:
	bool Init();
	void RenderBegin();
	void Render(const GameTimer& gt);
	
	~FRender();
private:
	std::unique_ptr<FRHI> mRHI = nullptr;
	std::unique_ptr<FRHIResource> mRHIResource = nullptr;
};