#include "stdafx.h"
#include "DX12Render.h"
#include "LauncherPCWindow.h"
#include "WindowsInputBase.h"

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return PCWindows::GetWindowsInput()->MsgProc(hwnd, msg, wParam, lParam);
}

PCWindows* PCWindows::pcWindows = nullptr;
//初始化塞入app和input
PCWindows::PCWindows(const std::shared_ptr<WindowsInputBase>& windowsInput)
{
	assert(pcWindows == nullptr);
	pcWindows = this;
	mWindowsInput = windowsInput;
}
PCWindows::~PCWindows()
{
	if (pcWindows!= nullptr)
	{
		pcWindows = nullptr;
	}
}
std::shared_ptr<WindowsInputBase> PCWindows::mWindowsInput = nullptr;
bool PCWindows::InitWindows()
{
	WNDCLASS wc;
	wc.hInstance = GetModuleHandle(0); ;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.lpszClassName = L"MainWnd";
	wc.lpszMenuName = 0;

	if (!RegisterClass(&wc)) {
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}
	RECT R = { 0,0,mClientWidht , mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

	int width = R.right - R.left;
	int heigth = R.bottom - R.top;
	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, heigth, 0, 0, GetModuleHandle(0), 0);
	if (!mhMainWnd) {
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}
	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
	return true;
}

HWND PCWindows::GetHWnd()
{
	return mhMainWnd;
}

int PCWindows::GetClientWidht()
{
	return mClientWidht;
}

int PCWindows::GetClientHeight()
{
	return mClientHeight;
}


PCWindows* PCWindows::GetPcWindows()
{
	return pcWindows;
}

std::shared_ptr<WindowsInputBase> PCWindows::GetWindowsInput()
{
	return mWindowsInput;
}



bool PCWindows::Run()
{
	bool Quit = false;
	MSG msg = { 0 };
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				Quit = true;
			}
		}
	return !Quit;
}

void PCWindows::CalculateFrameStats(const GameTimer& gt)
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;
	if ((gt.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);
		std::wstring timeStr = std::to_wstring(gt.TotalTime());

		std::wstring windowText = mMainWndCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr+
			L"   TotalTime: " + timeStr;

		SetWindowText(mhMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

