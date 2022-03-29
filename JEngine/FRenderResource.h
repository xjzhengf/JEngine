#pragma once
#include "stdafx.h"
#include "RHIResource.h"
class FRenderResource {
public:
	FRenderResource();
	FRenderResource(const FRenderResource& am) = delete;
	FRenderResource& operator=(const FRenderResource& am) = delete;
	~FRenderResource();
	virtual void ReleaseResource() {};
	static bool mRenderUpdate;
};