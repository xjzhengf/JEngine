#pragma once
#include "MeshProperty.h"
#include "FTexture.h"
class AssetManager
{
public:
	static AssetManager* GetAssetManager();

	AssetManager();
	AssetManager(const AssetManager& am) = delete;
	AssetManager& operator=(const AssetManager& am) = delete;
	~AssetManager();
	void ReadBinaryFileToStaticMeshStruct(const char* TextPath);

	StaticMeshInfo* FindAssetByActor(ActorStruct& actor);
	void LoadTexture();
	void SelectFile();
	std::unordered_map<std::string, StaticMeshInfo*>& GetMeshAsset();
	std::vector<std::shared_ptr<FTexture>>& GetTextures();
	std::set<std::string> AssetVector;
protected:
	static AssetManager* mAssetManager;
private:
	std::unordered_map<std::string, StaticMeshInfo*> MeshAsset;
	std::vector<std::shared_ptr<FTexture>> mTextures;
};


