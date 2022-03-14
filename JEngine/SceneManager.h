#pragma once
#include "stdafx.h"
#include "MeshProperty.h"
#include "Camera.h"
class SceneManager {
public:
	static SceneManager* GetSceneManager();

	SceneManager();
	~SceneManager();
	SceneManager(const SceneManager& sm) = delete;
	SceneManager& operator=(const SceneManager& sm) = delete;
	void SetMapActors(const std::unordered_map<std::string, ActorStruct*>& Actors);
	void ReadBinaryFileToActorStruct(const char* TextPathName);
	void RemoveActor(const std::string& Name);
	void AddActor( ActorStruct* Actor);
	std::shared_ptr<Camera> GetCamera();
	bool SceneManager::LoadMap(const char* MapPath);
	std::unordered_map<std::string, ActorStruct*>& GetAllActor();
protected:
	static SceneManager* mSceneManager;
private:
	std::unordered_map<std::string, ActorStruct*> Actors;
	std::shared_ptr<Camera> camera;
};