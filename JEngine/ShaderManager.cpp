#include "stdafx.h"
#include "ShaderManager.h"
ShaderManager* ShaderManager::mShaderManager = nullptr;
void ShaderManager::CreateShader(const std::wstring& filename)
{
#ifdef _WIN32
	mvsByteCode = d3dUtil::CompileShader(filename, nullptr, "VS", "vs_5_1");
	//mpsByteCode = d3dUtil::CompileShader(L"..\\JEngine\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");
	mpsByteCode = d3dUtil::CompileShader(filename, nullptr, "PS", "ps_5_1");
#endif
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
