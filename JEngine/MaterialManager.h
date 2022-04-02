#pragma once
#include "stdafx.h"
#include "Material.h"
//搜索Material
//缓存Material，搜索Material结果
//

class MaterialManager {
public:
	static MaterialManager* GetMaterialManager();
private:
#pragma region 缓存所有Material结果  

	std::map<std::string, FMaterial> mMaterialMap;
#pragma endregion
#pragma region 搜索Material结果
	const FMaterial& SearchMaterial(const std::string& MaterialName);
#pragma endregion

	MaterialManager();
	~MaterialManager();
	MaterialManager(const MaterialManager& sm) = delete;
	MaterialManager& operator=(const MaterialManager& sm) = delete;

protected:
	static MaterialManager* mMaterialManager;
};
