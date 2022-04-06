#pragma once
#include "stdafx.h"
//编译shader
//缓存shader结果，搜索shader结果，
//关联shader文件  
//组合shader文件

class FShader {
public:
#ifdef _WIN32
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
#endif
};
class ShaderManager {
public:
	static ShaderManager* GetShaderManager();
#pragma region 缓存shader结果  

	std::map<std::wstring, FShader> mShaderResult;
#pragma endregion
#pragma region 编译shader  搜索shader结果
public:
	 FShader* CompileShader(const std::wstring& filename);
#pragma endregion

	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager& sm) = delete;
	ShaderManager& operator=(const ShaderManager& sm) = delete;

protected:
	static ShaderManager* mShaderManager;
};

