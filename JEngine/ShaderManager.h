#pragma once
#include "stdafx.h"
class ShaderManager {
public:
	void CreateShader(const std::wstring& filename);
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
	static ShaderManager* GetShaderManager();
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager& sm) = delete;
	ShaderManager& operator=(const ShaderManager& sm) = delete;
protected:
	static ShaderManager* mShaderManager;
};
