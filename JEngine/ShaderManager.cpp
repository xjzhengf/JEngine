#include "stdafx.h"
#include "ShaderManager.h"
ShaderManager* ShaderManager::mShaderManager = nullptr;
FShader* ShaderManager::CompileShader(const std::wstring& filename)
{
	if (mShaderResult.find(filename)!= mShaderResult.end())
	{
		return &mShaderResult[filename];
	}
	FShader shader;
	mShaderResult.insert({filename,shader});

#ifdef _WIN32
	mShaderResult[filename].mvsByteCode = d3dUtil::CompileShader(filename, nullptr, "VS", "vs_5_1");
	mShaderResult[filename].mpsByteCode = d3dUtil::CompileShader(filename, nullptr, "PS", "ps_5_1");
#endif

	return &mShaderResult[filename];
}

ShaderManager* ShaderManager::GetShaderManager()
{
	return mShaderManager;
}

ShaderManager::ShaderManager()
{
	assert(mShaderManager == nullptr);
	mShaderManager = this;
}

ShaderManager::~ShaderManager()
{
	if (mShaderManager != nullptr) {
		mShaderManager = nullptr;
	}

}
