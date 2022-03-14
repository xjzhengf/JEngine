#include "stdafx.h"
#include "WindowsInput.h"
#include "TaskManager.h"


WindowsInput::WindowsInput()
{
	
}

LRESULT WindowsInput::MsgProc(HWND hwd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);
		//捕获消息，设置最小窗口大小
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		TaskManager::GetTaskManager()->RegisterMouse("MouseDown", lParam);
		//cameraInput->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		
		TaskManager::GetTaskManager()->RegisterMouse("MouseUp", lParam);
		//cameraInput->OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
	
		if ((wParam & MK_LBUTTON) != 0) {
			//TaskManager::GetTaskManager()->RegisterKey("LBUTMOUSEMOVE", lParam);
			TaskManager::GetTaskManager()->RegisterMouse("MouseMove", lParam);
		}
		if ((wParam & MK_RBUTTON) != 0) {
			TaskManager::GetTaskManager()->RegisterMouse("MouseMove", lParam);
			//TaskManager::GetTaskManager()->RegisterKey("RBUTMOUSEMOVE", lParam);
		}
		return 0;
	case WM_MOUSEWHEEL:
		//cameraInput->AddCameraSpeed(10.0f);
		TaskManager::GetTaskManager()->RegisterMouse("MouseWheel", lParam);
		return 0;
	case WM_KEYDOWN:
		TaskManager::GetTaskManager()->RegisterKey(wParam);
		
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}

		return 0;
	}

	return DefWindowProc(hwd, msg, wParam, lParam);
}
