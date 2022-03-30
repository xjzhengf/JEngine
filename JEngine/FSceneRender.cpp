#include "stdafx.h"
#include "FSceneRender.h"
#include "AssetManager.h"
#include "SceneManager.h"
const int gNumFrameResources = 3;
std::unordered_map<std::string, MeshData> FSceneRender::meshDataVector;
std::unordered_map<std::string, MeshData>& FSceneRender::BuildMeshData()
{
	if (!mRenderUpdate)
	{
		return meshDataVector;
	}
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		StaticMeshInfo* myStruct = AssetManager::GetAssetManager()->FindAssetByActor(*Actor.second);
		MeshData meshData;
		meshData.indices = myStruct->Indices;

		meshData.indices.resize(myStruct->Indices.size());
		size_t VerticesLen = myStruct->Vertices.size();
		meshData.vertices.resize(VerticesLen);
		for (int i = 0; i < VerticesLen; i++) {
			meshData.vertices[i].Pos.x = myStruct->Vertices[i].x;
			meshData.vertices[i].Pos.y = myStruct->Vertices[i].y;
			meshData.vertices[i].Pos.z = myStruct->Vertices[i].z;
			meshData.vertices[i].TexC.x = myStruct->UV[i].x;
			meshData.vertices[i].TexC.y = myStruct->UV[i].y;

		}

		for (size_t i = 0; i < (myStruct->Indices.size()) / 3; i++) {
			UINT i0 = meshData.indices[i * 3 + 0];
			UINT i1 = meshData.indices[i * 3 + 1];
			UINT i2 = meshData.indices[i * 3 + 2];

			Vertex v0 = meshData.vertices[i0];
			Vertex v1 = meshData.vertices[i1];
			Vertex v2 = meshData.vertices[i2];

			glm::vec3 e0 = v1.Pos - v0.Pos;
			glm::vec3 e1 = v2.Pos - v0.Pos;
			glm::vec3 faceNormal = glm::cross(e0, e1);

			meshData.vertices[i0].Normal += faceNormal;
			meshData.vertices[i1].Normal += faceNormal;
			meshData.vertices[i2].Normal += faceNormal;
		}

		for (UINT i = 0; i < VerticesLen; i++)
		{
			meshData.vertices[i].Normal = glm::normalize(meshData.vertices[i].Normal);
		}
		meshDataVector[Actor.first] = std::move(meshData);
		//meshDataVector.push_back(std::move(meshData));
	}
	mRenderUpdate = false;
	return meshDataVector;
}

void FSceneRender::BuildMaterial()
{
	auto grass = std::make_unique<FMaterial>();
	grass->Name = "Grass";
	grass->MatCBIndex = 0;
	grass->DiffuseAlbedo = glm::vec4(0.2f,0.6f,0.2f,1.0f);
	grass->FresnelR0 = glm::vec3(0.01f, 0.01f, 0.01f);
	grass->Roughness = 0.125f;

	mMaterials["Grass"] = std::move(grass);
}


