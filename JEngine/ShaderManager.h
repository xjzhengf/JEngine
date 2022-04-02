#pragma once
#include "stdafx.h"
//����shader
//����shader���������shader�����
//����shader�ļ�  
//���shader�ļ�

class FShader {
public:
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
};
class ShaderManager {
public:
	static ShaderManager* GetShaderManager();
#pragma region ����shader���  

	std::map<std::wstring, FShader> mShaderResult;
#pragma endregion
#pragma region ����shader  ����shader���
public:
	const FShader& CompileShader(const std::wstring& filename);
#pragma endregion

	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager& sm) = delete;
	ShaderManager& operator=(const ShaderManager& sm) = delete;

protected:
	static ShaderManager* mShaderManager;
};

