#include "stdafx.h"
#include "FTexture.h"



FTexture::FTexture()
{
    mFTexture = this;
}

void FTexture::ReleaseResource()
{
    delete mFTexture;
}
