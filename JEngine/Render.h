#pragma once
#include "stdafx.h"
#include "RHI.h"
class FRender {
public:
	bool Init();

	void Render(const GameTimer& gt);
	std::shared_ptr<FRHI> GetRHI();
	~FRender();
private:
	std::shared_ptr<FRHI> mRHI = nullptr;
};