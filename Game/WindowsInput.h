#pragma once
#include "stdafx.h"
#include "WindowsInputBase.h"
class WindowsInput : public WindowsInputBase{
public:
	WindowsInput();
	LRESULT MsgProc(HWND hwd, UINT msg, WPARAM wParam, LPARAM lParam) override;
};