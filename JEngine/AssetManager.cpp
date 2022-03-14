#include "stdafx.h"
#include "AssetManager.h"
#include "fstream"

AssetManager* AssetManager::mAssetManager = nullptr;
AssetManager* AssetManager::GetAssetManager()
{
	return mAssetManager;
}

AssetManager::AssetManager()
{
	assert(mAssetManager == nullptr);
	mAssetManager = this;
}

AssetManager::~AssetManager()
{
	for (auto&& data : MeshAsset)
	{
		if (data.second != nullptr) {
			delete data.second;
		}
	}

	if (mAssetManager != nullptr) {
		 mAssetManager = nullptr;
	}
}




void AssetManager::ReadBinaryFileToStaticMeshStruct(const char* TextPath)
{
	std::ifstream inFile(TextPath, std::ios::binary);
	StaticMeshInfo* meshAsset = new StaticMeshInfo;
	if (inFile.is_open()) {
		int flag = 0;
		inFile.read((char*)&flag, sizeof(int));
		int strlen = 0;
		inFile.read((char*)&strlen, sizeof(int32_t));
		meshAsset->StaticMeshName.resize(strlen);
		inFile.read((char*)meshAsset->StaticMeshName.data(), strlen * sizeof(char));

		inFile.read((char*)&meshAsset->NumVertices, sizeof(int32_t));
		inFile.read((char*)&meshAsset->NumLOD, sizeof(int32_t));
		inFile.read((char*)&meshAsset->NumTriangles, sizeof(int32_t));
		inFile.read((char*)&meshAsset->NumIndices, sizeof(int32_t));

		int verticesLen = 0;
		inFile.read((char*)&verticesLen, sizeof(int32_t));
		meshAsset->Vertices.resize(verticesLen);
		inFile.read((char*)meshAsset->Vertices.data(), verticesLen * sizeof(FVector));

		int indicesLen = 0;
		inFile.read((char*)&indicesLen, sizeof(int32_t));
		meshAsset->Indices.resize(indicesLen);
		inFile.read((char*)meshAsset->Indices.data(), indicesLen * sizeof(uint32_t));

		
		int ColorVerticesLen = 0;
		inFile.read((char*)&ColorVerticesLen, sizeof(int32_t));
		meshAsset->ColorVertices.resize(ColorVerticesLen);
		inFile.read((char*)meshAsset->ColorVertices.data(), ColorVerticesLen * sizeof(FColor));
	}
	inFile.close();
	this->MeshAsset.insert({ meshAsset->StaticMeshName,meshAsset });
	AssetVector.insert(meshAsset->StaticMeshName);
}



StaticMeshInfo* AssetManager::FindAssetByActor(ActorStruct& actor)
{
	//虚幻导出的资源字符串后缀'/0'
	std::string str(actor.StaticMeshAssetName[0].c_str());
	str.resize(str.size() + 1);
	auto iter = MeshAsset.find(str);

	if (iter != MeshAsset.end()) {
		return iter->second;
	}
	return nullptr;
}

void AssetManager::SelectFile()
{
	char* TextPathName = new char;
	int len;
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All(*.*)\0*.*\0Text(*.txt)\0*.TXT\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		len = WideCharToMultiByte(CP_ACP, 0, szFile, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, szFile, -1, TextPathName, len, NULL, NULL);
		std::ifstream inFile(TextPathName, std::ios::binary);
		if (inFile.is_open()) {
			int flag = 0;
			inFile.read((char*)&flag, sizeof(int));
			inFile.close();
			if (flag) {

			}
			else {
				ReadBinaryFileToStaticMeshStruct(TextPathName);
			}
		}
	}
	delete TextPathName;
}

std::unordered_map<std::string, StaticMeshInfo*>& AssetManager::GetMeshAsset()
{
	return MeshAsset;
}



