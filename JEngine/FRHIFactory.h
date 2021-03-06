#pragma once
#include "stdafx.h"
#include "FDynamicRHI.h"
#include "RHIResource.h"
class FRHIFactory {
public:
	std::shared_ptr<FDynamicRHI> CreateRHI();
	std::shared_ptr<FRHIResource> CreateRHIResource();
	std::shared_ptr<FRenderResource> CreateShadowResource();
	std::shared_ptr<FRenderResource> CreateHDRResource();
};