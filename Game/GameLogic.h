#pragma once
#include "stdafx.h"
#include "Engine.h"
class GameLogic {
public:
	GameLogic();
	GameLogic(const GameLogic& GL) = delete;
	GameLogic& operator=(const GameLogic& GL) = delete;
	~GameLogic();
	void Init();
	void Update();
	void Destroy();
	static GameLogic* GetGameLogic();
	bool LoadMap(const std::string& PathName);
protected:
	static GameLogic* mGameLogic;
private:
	void ProcessKey();
	void ProcessMouse();
};