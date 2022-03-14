#include "stdafx.h"
#include "TaskManager.h"

TaskManager* TaskManager::mTaskManager = nullptr;
TaskManager::TaskManager(HINSTANCE hInstance)
{
	assert(mTaskManager == nullptr);
	mTaskManager = this;
}

TaskManager* TaskManager::GetTaskManager()
{
	return mTaskManager;
}

void TaskManager::RunInput(const std::string& FuncName)
{
	isInput = true;
	for (const auto& EventPair : EventMapInString)
	{
		auto& Event = EventPair.second;
		if (Event) {
			Event(FuncName);
		}
	}
	for (const auto& Name : PrepareRemove) {
		EventMapInString.erase(Name);
	}
	isInput = false;
}

void TaskManager::Register(const std::string& Name, CallBackInString Callback)
{
	EventMapInString.insert({Name,Callback});
}

void TaskManager::RegisterKey(const WPARAM& wParam)
{
	EventKey.insert(wParam);
}

void TaskManager::UnRegisterKey(const WPARAM& wParam)
{
	PrepareRemoveKey.insert(wParam);
}

void TaskManager::ClearImplKey()
{
	for (auto RemoveKey : PrepareRemoveKey)
	{
		EventKey.erase(RemoveKey);
	}
	PrepareRemoveKey.clear();
}

void TaskManager::RegisterMouse(const std::string& MouseKey, LPARAM lParam)
{
	EventMouseKeyMap.insert({ MouseKey, lParam });
}

void TaskManager::UnRegister(const std::string& Name)
{
	if (isInput) {
		PrepareRemove.push_back(Name);
	}
	else
	{
		EventMapInString.erase(Name);
	}
}
