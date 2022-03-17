#include "stdafx.h"
#include "FTexture.h"


void FTexture::SetResoureInMap(std::string name)
{
	mFTexture = std::make_shared<FTexture>();
	mFTexture->Name = this->Name;
	mFTexture->FilePath = this->FilePath;
	ResoureMap.insert({name,mFTexture});
}
