#pragma once
#include "stdafx.h"
class FResource {
public:
	ID3D12Resource* Resource;
};
class FRHIResource {
public:
	virtual void Release() = 0;
	virtual unsigned __int64  CurrentBackBufferViewHand() { return 0; }
	virtual unsigned __int64  CurrentDepthStencilViewHand() { return 0; }
	virtual std::shared_ptr<FResource> BackBuffer() = 0;
};