#include "stdafx.h"
#include "Engine.h"
#include "WindowsFactory.h"
#include "LauncherPCWindow.h"
#include "FirstPersonCamera.h"
#include "GameLogic.h"
#include "WindowsInput.h"
#include <thread>
Engine* Engine::mEngine = nullptr;
Engine::Engine()
{
	assert(mEngine == nullptr);
	mEngine = this;
}
Engine::~Engine()
{
	if (mEngine != nullptr) {
		mEngine = nullptr;
	}
}
void Engine::Init(HINSTANCE hInstance)
{
	//��ʼ������
	std::unique_ptr<WindowsFactory> fa = std::make_unique<WindowsFactory>();
	//��ȡPCWindows
	mWindows = fa->GetWindows();
	mWindows->InitWindows();

	//��ʼ��Render
	mRender = std::make_unique<DX12Render>();

	//��ʼ����Դ����ͳ�������
	mAssetManager = std::make_shared<AssetManager>();
	mSceneManager = std::make_shared<SceneManager>();

	//��ʼ��Render
	mRender->Initialize();
	//�����������ϵͳ
	mTaskManager = std::make_unique<TaskManager>(GetModuleHandle(0));
}

void Engine::Run(GameTimer& gt)
{
	gt.Reset();
	isRuning = true;
//��ʼ��ѭ��
	while (isRuning && mWindows->Run())
	{
		//GameTick
		GameLogic::GetGameLogic()->Update();
		//RenderTick
		RenderTick(gt);
	}
}

void Engine::RenderTick(GameTimer& gt)
{
	mWindows->CalculateFrameStats(gt);
	gt.Tick();
	if (!mRender->GetAppPause()) {
		mRender->Update(gt);
		mRender->Draw(gt);
	}
	else
	{
		Sleep(100);
	}
}



void Engine::Destroy()
{
	if (mRender != nullptr) {
	    mRender= nullptr;
	}
	if (mWindows != nullptr) {
		mWindows = nullptr;
	}

	if (mTaskManager != nullptr) {
		mTaskManager = nullptr;
	}
	if (mSceneManager != nullptr) {
		mSceneManager = nullptr;
	}
	if (mAssetManager != nullptr) {
		mAssetManager = nullptr;
	}
	
}

std::shared_ptr<AssetManager> Engine::GetAssetManager()
{
	return mAssetManager;
}

std::shared_ptr<SceneManager> Engine::GetSceneManager()
{
	return mSceneManager;
}

void Engine::UpdateDrawState(bool state)
{
	mRender->isUpdateDraw = state;
}

std::shared_ptr<WindowBase> Engine::GetWindow()
{
	return mWindows;
}


bool Engine::GetRuningState()
{
	return isRuning;
}

void Engine::SetRuningState(bool state)
{
	isRuning = state;
}

Engine* Engine::GetEngine()
{
	return mEngine;
}
