#pragma once
#include "GameTimer.h"
class WindowBase
{
public:
	virtual bool InitWindows() =0;
	virtual bool Run() = 0;
	virtual void CalculateFrameStats(const GameTimer& gt) = 0;
	virtual HWND GetHWnd() =0;
	virtual int GetClientWidht() = 0;
	virtual int GetClientHeight() = 0;
};

