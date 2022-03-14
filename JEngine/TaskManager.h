#pragma once
#include "stdafx.h"


class TaskManager
{
public:
	std::unordered_map<std::string, CallBackInString> EventMapInString;
	std::set<WPARAM> EventKey;
	
	std::map<std::string, LPARAM> EventMouseKeyMap;

	std::vector<std::string> PrepareRemove;
	std::set<WPARAM>  PrepareRemoveKey;
	TaskManager(HINSTANCE hInstance);
	TaskManager(const TaskManager& tm) = delete;
	TaskManager& operator=(const TaskManager& tm) = delete;
	static TaskManager* GetTaskManager();

	void RunInput(const std::string& FuncName);
	void Register(const std::string& Name, CallBackInString Callback);
	void RegisterKey(const WPARAM& wParam);
	void UnRegisterKey(const WPARAM& wParam);
	void ClearImplKey();
	void RegisterMouse(const std::string& MouseKey, LPARAM lParam);
	void UnRegister(const std::string& Name);
private:
	bool isInput = false;
protected:
	static TaskManager* mTaskManager;
};

