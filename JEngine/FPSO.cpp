#include "stdafx.h"
#include "FPSO.h"
#include "ShaderManager.h"
PipelineState FPSO::CreateFPSO(const std::string& PSOType, std::vector<INPUT_ELEMENT_DESC> mInputLayout,FShader* shader)
{
	if (mPsoMap.find(PSOType)!=mPsoMap.end()) {
		return mPsoMap[PSOType];
	}
	if (PSOType == "Scene") {
		mPsoMap[PSOType]= BuildRenderFPSO(mInputLayout, shader);
	}
	if (PSOType == "ShadowMap") {
		mPsoMap[PSOType]= BuildDepthFPSO(mInputLayout, shader);
	}
	return mPsoMap[PSOType];
}

PipelineState FPSO::BuildRenderFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader)
{
	PipelineState mPso;
#ifdef _WIN32
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;  
	std::vector<D3D12_INPUT_ELEMENT_DESC> dxInputLayout;
	mInputLayout =
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, FORMAT_R32G32B32A32_FLOAT, 0, 12, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 28, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 40, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
	dxInputLayout.resize(mInputLayout.size());
	for (int i =0 ; i<mInputLayout.size();i++)
	{
		dxInputLayout[i].AlignedByteOffset =std::move(mInputLayout[i].AlignedByteOffset);
		dxInputLayout[i].Format = std::move(DXGI_FORMAT(mInputLayout[i].Format));
		dxInputLayout[i].InputSlot = std::move(mInputLayout[i].InputSlot);
		dxInputLayout[i].InputSlotClass = std::move(D3D12_INPUT_CLASSIFICATION(mInputLayout[i].InputSlotClass));
		dxInputLayout[i].InstanceDataStepRate = std::move(mInputLayout[i].InstanceDataStepRate);
		dxInputLayout[i].SemanticIndex = std::move(mInputLayout[i].SemanticIndex);
		dxInputLayout[i].SemanticName = std::move(mInputLayout[i].SemanticName);
	}
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { dxInputLayout.data(),(UINT)dxInputLayout.size() };

	psoDesc.VS = {
		reinterpret_cast<BYTE*>(shader->mvsByteCode->GetBufferPointer()),
		shader->mvsByteCode->GetBufferSize()
	};
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(shader->mpsByteCode->GetBufferPointer()),
		shader->mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	mPso.dxPSO =std::move(psoDesc);
	return mPso;
#endif
	return PipelineState();
}

PipelineState FPSO::BuildDepthFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader)
{
	PipelineState mPso;
#ifdef _WIN32	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC> dxInputLayout;
	mInputLayout =
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	dxInputLayout.resize(mInputLayout.size());
	for (int i = 0; i < mInputLayout.size(); i++)
	{
		dxInputLayout[i].AlignedByteOffset = std::move(mInputLayout[i].AlignedByteOffset);
		dxInputLayout[i].Format = std::move(DXGI_FORMAT(mInputLayout[i].Format));
		dxInputLayout[i].InputSlot = std::move(mInputLayout[i].InputSlot);
		dxInputLayout[i].InputSlotClass = std::move(D3D12_INPUT_CLASSIFICATION(mInputLayout[i].InputSlotClass));
		dxInputLayout[i].InstanceDataStepRate = std::move(mInputLayout[i].InstanceDataStepRate);
		dxInputLayout[i].SemanticIndex = std::move(mInputLayout[i].SemanticIndex);
		dxInputLayout[i].SemanticName = std::move(mInputLayout[i].SemanticName);
	}
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { dxInputLayout.data(),(UINT)dxInputLayout.size() };

	psoDesc.VS = {
		reinterpret_cast<BYTE*>(shader->mvsByteCode->GetBufferPointer()),
		shader->mvsByteCode->GetBufferSize()
	};
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(shader->mpsByteCode->GetBufferPointer()),
		shader->mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.RasterizerState.DepthBias = 100000;
	psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	psoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 0;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	mPso.dxPSO = std::move(psoDesc);
	return mPso;
#endif
	return PipelineState();
}
