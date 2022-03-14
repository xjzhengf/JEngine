#pragma once
#include "stdafx.h"
#include "Engine.h"
#include "GameLogic.h"
class GameInstance {
public:
	void Init(HINSTANCE hInstance);
	void UpDate();
	void Destroy();

private:
	std::unique_ptr<Engine> mEngine;
	std::unique_ptr<GameLogic> mGameLogic;

protected:
	GameTimer mTimer;
};