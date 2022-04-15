#include "stdafx.h"
#include "MaterialManager.h"
#include "FPSO.h"
MaterialManager* MaterialManager::mMaterialManager = nullptr;
MaterialManager* MaterialManager::GetMaterialManager()
{
	return mMaterialManager;
}

const FMaterial& MaterialManager::SearchMaterial(const std::string& MaterialName)
{
	//if (!mMaterialMap.empty()) {
		if (mMaterialMap.find(MaterialName) != mMaterialMap.end())
		{
			return mMaterialMap[MaterialName];
		}
	//}
	if (MaterialName == "BloomSet") {
		std::vector<INPUT_ELEMENT_DESC> mInputLayout;
		mInputLayout =
		{
			{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{"TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 12, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"UV", 0, FORMAT_R32G32_FLOAT, 0, 24, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}

		};
		material.Name = "BloomSet";
		material.GlobalShader = L"..\\JEngine\\Shaders\\color.hlsl";
		material.mPso = pso->CreateFPSO("BloomSet", std::move(mInputLayout), ShaderManager::GetShaderManager()->CompileShader(L"..\\JEngine\\Shaders\\color.hlsl"));		
		mMaterialMap["BloomSet"] = material;
		return mMaterialMap["BloomSet"];
	}
	if (MaterialName == "BloomDown") {
		std::vector<INPUT_ELEMENT_DESC> mInputLayout;
		mInputLayout =
		{
			{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{"TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 12, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"UV", 0, FORMAT_R32G32_FLOAT, 0, 24, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}

		};
		material.Name = "BloomDown";
		material.GlobalShader = L"..\\JEngine\\Shaders\\BloomDown.hlsl";
		material.mPso = pso->CreateFPSO("BloomDown", std::move(mInputLayout), ShaderManager::GetShaderManager()->CompileShader(L"..\\JEngine\\Shaders\\BloomDown.hlsl"));
		mMaterialMap["BloomDown"] = material;
		return mMaterialMap["BloomDown"];
	}
	if (MaterialName == "BloomUp") {
		std::vector<INPUT_ELEMENT_DESC> mInputLayout;
		mInputLayout =
		{
			{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{"TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 12, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"UV", 0, FORMAT_R32G32_FLOAT, 0, 24, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}

		};
		material.Name = "BloomUp";
		material.GlobalShader = L"..\\JEngine\\Shaders\\BloomUp.hlsl";
		material.mPso = pso->CreateFPSO("BloomUp", std::move(mInputLayout), ShaderManager::GetShaderManager()->CompileShader(L"..\\JEngine\\Shaders\\BloomUp.hlsl"));
		mMaterialMap["BloomUp"] = material;
		return mMaterialMap["BloomUp"];
	}
	if (MaterialName == "ShadowMap") {

		std::vector<INPUT_ELEMENT_DESC> mInputLayout;
		mInputLayout =
		{
			{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		material.Name = "ShadowMap";
		material.GlobalShader = L"..\\JEngine\\Shaders\\Shadow.hlsl";
		material.mPso = pso->CreateFPSO("ShadowMap", std::move(mInputLayout), ShaderManager::GetShaderManager()->CompileShader(L"..\\JEngine\\Shaders\\Shadow.hlsl"));
		mMaterialMap["ShadowMap"] = material;
		return mMaterialMap["ShadowMap"];
	}
	if (MaterialName == "Bloom") {

		std::vector<INPUT_ELEMENT_DESC> mInputLayout;
		mInputLayout =
		{
			{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, FORMAT_R32G32B32A32_FLOAT, 0, 12, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 28, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{"TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 40, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, FORMAT_R32G32B32_FLOAT, 0, 48, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};
		material.Name = "Bloom";
		material.GlobalShader = L"..\\JEngine\\Shaders\\HightLight.hlsl";
		material.mPso = pso->CreateFPSO("Bloom", std::move(mInputLayout), ShaderManager::GetShaderManager()->CompileShader(L"..\\JEngine\\Shaders\\HightLight.hlsl"));
		mMaterialMap["Bloom"] = material;

		return mMaterialMap["Bloom"];
	}
	return mMaterialMap["Default"];
}

MaterialManager::MaterialManager()
{
	assert(mMaterialManager == nullptr);
	mMaterialManager = this;
}

MaterialManager::~MaterialManager()
{

	std::vector<INPUT_ELEMENT_DESC> mInputLayout;
	mInputLayout =
	{
			{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, FORMAT_R32G32B32A32_FLOAT, 0, 12, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 28, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{"TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 40, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, FORMAT_R32G32B32_FLOAT, 0, 48, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
	material.GlobalShader = L"..\\JEngine\\Shaders\\color.hlsl";
	material.Name = "Default";
	material.mMaterialConstants.DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	material.mMaterialConstants.FresnelR0 = { 0.5f, 0.5f, 0.5f };
	material.mMaterialConstants.Roughness = 0.01f;
	material.mPso = pso->CreateFPSO("Scene", std::move(mInputLayout), ShaderManager::GetShaderManager()->CompileShader(L"..\\JEngine\\Shaders\\color.hlsl"));
	mMaterialMap["Default"] = material;
}
