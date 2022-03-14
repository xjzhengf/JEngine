#pragma once
#include "stdafx.h"
#include "WindowBase.h"
#include "WindowsInputBase.h"
class WindowsFactory
{
public:

	std::shared_ptr<WindowBase> GetWindows();
private:

};

