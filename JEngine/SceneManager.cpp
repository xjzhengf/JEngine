#include "stdafx.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include"FirstPersonCamera.h"
SceneManager* SceneManager::mSceneManager = nullptr;
SceneManager* SceneManager::GetSceneManager()
{
	return mSceneManager;
}

SceneManager::SceneManager()
{
	assert(mSceneManager == nullptr);
	mSceneManager = this;
	camera = std::make_shared<FirstPersonCamera>();
}

SceneManager::~SceneManager()
{
	if (mSceneManager != nullptr) {
		 mSceneManager = nullptr;
	}
	for (auto&& data : Actors)
	{
		if (data.second != nullptr) {
			delete data.second;
		}
	}
}

void SceneManager::SetMapActors(const std::unordered_map<std::string, ActorStruct*>& ActorMap)
{
	Actors.clear();
	for (auto&& Actor : ActorMap) {
     	Actors.insert({ Actor.first,Actor.second });
	}
}
bool SceneManager::LoadMap(const char* MapPath)
{
	Actors.clear();
	std::ifstream inFile(MapPath, std::ios::in);
	std::string buf;
	std::vector<std::string> ActorsPath;
	if (inFile.is_open()) {
		while (getline(inFile, buf))
		{
			ActorsPath.push_back(buf);
		}
	}
	else {
		return false;
	}
	for (std::string& ActorPath : ActorsPath) {
		ReadBinaryFileToActorStruct(ActorPath.c_str());
	}
	return true;
}

void SceneManager::RemoveActor(const std::string& Name)
{
	Actors.erase(Name);
}

void SceneManager::AddActor(ActorStruct* Actor)
{
	Actors.insert({ Actor->ActorName,Actor });
}

std::shared_ptr<Camera> SceneManager::GetCamera()
{
	return camera;
}

std::unordered_map<std::string, ActorStruct*>& SceneManager::GetAllActor()
{
	return Actors;
}
void SceneManager::ReadBinaryFileToActorStruct(const char* TextPathName)
{

	int ComponentLen = 0;
	int Meshlen = 0;
	ActorStruct* actor = new ActorStruct;
	std::ifstream inFile(TextPathName, std::ios::binary);
	if (inFile.is_open()) {
		int flag = 0;
		inFile.read((char*)&flag, sizeof(int));
		int len = 0;
		inFile.read((char*)&len, sizeof(int32_t));
		actor->ActorName.resize(actor->ActorName.size() + len * sizeof(char));
		inFile.read((char*)actor->ActorName.data(), len);

		inFile.read((char*)&ComponentLen, sizeof(int32_t));
		actor->Transform.resize(ComponentLen);
		inFile.read((char*)actor->Transform.data(), ComponentLen * sizeof(FTransform));


		inFile.read((char*)&Meshlen, sizeof(int32_t));
		actor->StaticMeshAssetName.resize(Meshlen);
		for (size_t i = 0; i < Meshlen; i++)
		{
			int strlen = 0;
			inFile.read((char*)&strlen, sizeof(int32_t));
			inFile.read((char*)actor->StaticMeshAssetName[i].data(), strlen);
		}

	}
	inFile.close();
	std::string assetPath;
	for (int i = 0; i < actor->StaticMeshAssetName.size(); i++) {
		//虚幻导出的资源字符串后缀'/0'
		std::string str(actor->StaticMeshAssetName[0].c_str());
		str.resize(str.size() + 1);
		if (AssetManager::GetAssetManager()->AssetVector.find(str) == AssetManager::GetAssetManager()->AssetVector.end()) {
			assetPath = std::string("..\\JEngine\\StaticMeshInfo\\").append(actor->StaticMeshAssetName[0].c_str()).append(".dat");
			AssetManager::GetAssetManager()->ReadBinaryFileToStaticMeshStruct(assetPath.c_str());
		}
	}
	Actors.insert({ actor->ActorName,actor });
}