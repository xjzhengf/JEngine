#include "stdafx.h"
#include "FDynamicRHI.h"
#include "DX12RHI.h"

FDynamicRHI* FDynamicRHI::mRHI = nullptr;
FDynamicRHI::FDynamicRHI()
{
}

FDynamicRHI* FDynamicRHI::GetRHI()
{
	return mRHI;
}

void FDynamicRHI::DestroyRHI()
{
	delete mRHI;
}
