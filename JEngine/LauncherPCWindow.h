#pragma once
#include "stdafx.h"
#include "WindowBase.h"
#include "WindowsInputBase.h"
class PCWindows :public WindowBase{
public:
	PCWindows(const std::shared_ptr<WindowsInputBase>& windowsInput);
	~PCWindows();
	virtual bool InitWindows() override;

	HWND GetHWnd()override;
	int GetClientWidht()override;
	int GetClientHeight()override;

	static PCWindows* GetPcWindows();
	static std::shared_ptr<WindowsInputBase> GetWindowsInput();

	virtual bool Run() override;

	void CalculateFrameStats(const GameTimer& gt) override;
protected:
	GameTimer mTimer;
	static std::shared_ptr<WindowsInputBase> mWindowsInput;
	static PCWindows* pcWindows;
private:	
	   HINSTANCE mhAppInst = nullptr;
	   HWND mhMainWnd = nullptr;
	   bool mAppPause = false;         //是否暂停
	   bool mMinimized = false;        //最小化
	   bool mMaximized = false;        //最大化
	   bool mResizing = false;         //调整大小是否被拖动
	   bool mFullscreenState = false;  //开启全屏
	   std::wstring mMainWndCaption = L"My Windows App";
	   int mClientWidht = 800;
	   int mClientHeight = 600;
};