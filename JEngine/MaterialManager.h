#pragma once
#include "stdafx.h"
#include "Material.h"
//????Material
//????Material??????Material????
//

class MaterialManager {
public:
	static MaterialManager* GetMaterialManager();
private:
#pragma region ????????Material????  

	std::map<std::string, FMaterial> mMaterialMap;
#pragma endregion
public:
#pragma region ????Material????
	const FMaterial& SearchMaterial(const std::string& MaterialName);
#pragma endregion
public:
	MaterialManager();
	~MaterialManager();
	MaterialManager(const MaterialManager& sm) = delete;
	MaterialManager& operator=(const MaterialManager& sm) = delete;

protected:
	static MaterialManager* mMaterialManager;
	FMaterial material;
	std::unique_ptr<FPSO> pso = std::make_unique<FPSO>();
};
