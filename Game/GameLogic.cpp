#include "stdafx.h"
#include "GameLogic.h"
#include "AssetManager.h"
#include "TaskManager.h"

GameLogic* GameLogic::mGameLogic = nullptr;
GameLogic::GameLogic()
{
	assert(mGameLogic == nullptr);
	mGameLogic=this;
}

GameLogic::~GameLogic()
{
	if (mGameLogic != nullptr) {
		mGameLogic = nullptr;
	}
}

void GameLogic::Init()
{
	LoadMap("..\\JEngine\\StaticMeshInfo\\Map\\ThirdPersonMap.txt");
	LoadLight("..\\JEngine\\StaticMeshInfo\\Light\\LightSource_0.dat");
}

void GameLogic::Update()
{
	ProcessMouse();
	ProcessKey();
}

void GameLogic::Destroy()
{
}

GameLogic* GameLogic::GetGameLogic()
{
	return mGameLogic;
}

bool GameLogic::LoadMap(const std::string& PathName)
{
	if (Engine::GetEngine()->GetSceneManager()->LoadMap(PathName.c_str())) {
		//LoadTexture
		Engine::GetEngine()->GetAssetManager()->LoadTexture();
		return true;
	}
	return false;
}

bool GameLogic::LoadLight(const std::string& PathName)
{
	if (Engine::GetEngine()->GetSceneManager()->ReadBinaryFileToDirectionalLight(PathName.c_str())) {
		return true;
	}
	return false;
}

void GameLogic::ProcessKey()
{
	if (!TaskManager::GetTaskManager()->EventKey.empty()) {
		for (auto&& Key : TaskManager::GetTaskManager()->EventKey) {

			if (SceneManager::GetSceneManager()->GetCamera()->CameraMove("", Key, 0)) {
				TaskManager::GetTaskManager()->UnRegisterKey(Key);
			}
#ifdef _WIN32 
			if (Key == VK_ESCAPE) {
				Engine::GetEngine()->SetRuningState(false);
			}
			//ÔÝÊ±ÓÐbug
			//if (Key == VK_TAB)
			//{
			//	if (LoadMap("StaticMeshInfo\\Map\\ThirdPersonMap2.txt"))
			//		TaskManager::GetTaskManager()->UnRegisterKey(Key);
			//}
#else
			
#endif
		}
		TaskManager::GetTaskManager()->ClearImplKey();
	}
}

void GameLogic::ProcessMouse()
{
	if (!TaskManager::GetTaskManager()->EventMouseKeyMap.empty())
	{
		for (auto&& MouseKey : TaskManager::GetTaskManager()->EventMouseKeyMap) {
			SceneManager::GetSceneManager()->GetCamera()->CameraMove(MouseKey.first, NULL, MouseKey.second);
		}
		TaskManager::GetTaskManager()->EventMouseKeyMap.clear();

	}
}
