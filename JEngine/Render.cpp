#include "stdafx.h"
#include "Render.h"
#include "DX12RHI.h"
bool FRender::Init()
{
	mRHI = std::make_shared<DX12RHI>();
	if (!mRHI->Initialize()) {
		return false;
	}
	return true;
}

void FRender::Render(const GameTimer& gt)
{
	mRHI->Update(gt);
	mRHI->Draw(gt);
}

std::shared_ptr<FRHI> FRender::GetRHI()
{
	return mRHI;
}

FRender::~FRender()
{
	if (mRHI != nullptr) {
		mRHI = nullptr;
	}
}
