#pragma once
#include "stdafx.h"
//����shader
//����shader���������shader�����
//����shader�ļ�  
//���shader�ļ�

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
#pragma region ����shader���  

	std::map<std::wstring, FShader> mShaderResult;
#pragma endregion
#pragma region ����shader  ����shader���
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

