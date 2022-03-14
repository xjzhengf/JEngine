#include "stdafx.h"
#include "GameInstance.h"

void GameInstance::Init(HINSTANCE hInstance)
{
	mEngine = std::make_unique<Engine>();
	mGameLogic = std::make_unique<GameLogic>();
	mEngine->Init(hInstance);
	mGameLogic->Init();
}

void GameInstance::UpDate()
{
	mEngine->Run(mTimer);
}

void GameInstance::Destroy()
{

	mGameLogic->Destroy();
	mEngine->Destroy();
}
