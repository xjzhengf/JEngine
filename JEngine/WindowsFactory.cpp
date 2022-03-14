#include "stdafx.h"
#include "WindowsFactory.h"
#include "WindowsInput.h"
#include "LauncherPCWindow.h"

std::shared_ptr<WindowBase> WindowsFactory::GetWindows()
{
#ifdef _WIN32 
	std::shared_ptr<WindowsInputBase> windowsInput = std::make_shared<WindowsInput>();
	return std::make_shared<PCWindows>(windowsInput);
#else 
	return nullptr;
#endif
}
