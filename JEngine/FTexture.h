#pragma once
#include "stdafx.h"
#include "RHIResource.h"

class FTexture : public FRHIResource {
public:
	virtual void SetResoureInMap(std::string name)override;
	std::string Name;
	std::wstring FilePath;
private:
	std::shared_ptr<FTexture> mFTexture;
};