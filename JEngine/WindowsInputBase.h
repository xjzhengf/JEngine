#pragma once
#include "stdafx.h"
class WindowsInputBase {
public:
	virtual LRESULT MsgProc(HWND hwd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};