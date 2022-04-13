#pragma once
#include "stdafx.h"
#include "FRenderResource.h"

class FTexture  {
public:
	FTexture();
	std::string Name;
	std::wstring FilePath;
private:
	FTexture* mFTexture;
};

