#pragma once
#include "stdafx.h"
#include "FRenderResource.h"

class FTexture : public FRenderResource {
public:
	FTexture();
	virtual void ReleaseResource()override;
	std::string Name;
	std::wstring FilePath;
private:
	FTexture* mFTexture;
};

