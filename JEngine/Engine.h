#pragma once
#include "stdafx.h"
#include "Render.h"
#include "WindowBase.h"
#include "AssetManager.h"
#include "TaskManager.h"
#include "SceneManager.h"
#include "ShaderManager.h"
class Engine
{
public:
	Engine();
	~Engine();
	void Init(HINSTANCE hInstance);
	void Run(GameTimer& gt);
	void RenderTick(GameTimer& gt);
	void Destroy();
	static Engine* GetEngine();
	std::shared_ptr<AssetManager> GetAssetManager();
	std::shared_ptr<SceneManager> GetSceneManager();
	std::shared_ptr<WindowBase> GetWindow();
	bool GetRuningState();
	void SetRuningState(bool state);
	float Time;
protected:

	static Engine* mEngine;
private:
	std::unique_ptr<FRender> mRender;
	std::shared_ptr<WindowBase> mWindows;
	std::shared_ptr<AssetManager> mAssetManager;
	std::shared_ptr<SceneManager> mSceneManager;
	std::unique_ptr<TaskManager> mTaskManager;
	std::unique_ptr<ShaderManager> mShaderManager;
	bool isRuning;
};