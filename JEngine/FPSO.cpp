#include "stdafx.h"
#include "FPSO.h"
#include "ShaderManager.h"
FPSO::FPSO()
{
}
const PipelineState& FPSO::CreateFPSO(const std::string& PSOType, std::vector<INPUT_ELEMENT_DESC> mInputLayout,FShader* shader)
{
	if (mPsoMap.find(PSOType)!=mPsoMap.end()) {
		return mPsoMap[PSOType];
	}
	if ( PSOType == "Glitch") {
		mPsoMap[PSOType]= BuildRenderFPSO(mInputLayout, shader);
	}
	if (PSOType == "ShadowMap") {
		mPsoMap[PSOType]= BuildDepthFPSO(mInputLayout, shader);
	}
	if (PSOType == "Bloom") {
		mPsoMap[PSOType] = BuildBloomFPSO(mInputLayout, shader);
	}
	if (PSOType == "BloomSet") {
		mPsoMap[PSOType] = BuildBloomSetFPSO(mInputLayout, shader);
	}
	if (PSOType == "BloomDown") {
		mPsoMap[PSOType] = BuildBloomDownFPSO(mInputLayout, shader);
	}	
	if (PSOType == "BloomUp" || PSOType == "SunMerge" ||PSOType == "ToneMap"  ) {
		mPsoMap[PSOType] = BuildBloomUpFPSO(mInputLayout, shader, PSOType);
	}
	return mPsoMap[PSOType];
}

PipelineState FPSO::BuildRenderFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader)
{

#ifdef _WIN32
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;  
	dxInputLayout.resize(mInputLayout.size());
	for (int i =0 ; i<mInputLayout.size();i++)
	{
		dxInputLayout[i].AlignedByteOffset =mInputLayout[i].AlignedByteOffset;
		dxInputLayout[i].Format = DXGI_FORMAT(mInputLayout[i].Format);
		dxInputLayout[i].InputSlot = mInputLayout[i].InputSlot;
		dxInputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION(mInputLayout[i].InputSlotClass);
		dxInputLayout[i].InstanceDataStepRate = mInputLayout[i].InstanceDataStepRate;
		dxInputLayout[i].SemanticIndex = mInputLayout[i].SemanticIndex;
		dxInputLayout[i].SemanticName = mInputLayout[i].SemanticName;
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
	mPso.PSOName = "Glitch";
	mPso.dxPSO =psoDesc;
	return mPso;
#endif
	return PipelineState();
}

PipelineState FPSO::BuildDepthFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader)
{
#ifdef _WIN32	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;

	dxInputLayout.resize(mInputLayout.size());
	for (int i = 0; i < mInputLayout.size(); i++)
	{
		dxInputLayout[i].AlignedByteOffset = mInputLayout[i].AlignedByteOffset;
		dxInputLayout[i].Format = DXGI_FORMAT(mInputLayout[i].Format);
		dxInputLayout[i].InputSlot = mInputLayout[i].InputSlot;
		dxInputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION(mInputLayout[i].InputSlotClass);
		dxInputLayout[i].InstanceDataStepRate = mInputLayout[i].InstanceDataStepRate;
		dxInputLayout[i].SemanticIndex = mInputLayout[i].SemanticIndex;
		dxInputLayout[i].SemanticName = mInputLayout[i].SemanticName;
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
	mPso.dxPSO = psoDesc;
	mPso.PSOName = "ShadowMap";
	return mPso;
#endif
	return PipelineState();
}

PipelineState FPSO::BuildBloomFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader)
{
#ifdef _WIN32	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;

	dxInputLayout.resize(mInputLayout.size());
	for (int i = 0; i < mInputLayout.size(); i++)
	{
		dxInputLayout[i].AlignedByteOffset = mInputLayout[i].AlignedByteOffset;
		dxInputLayout[i].Format = DXGI_FORMAT(mInputLayout[i].Format);
		dxInputLayout[i].InputSlot = mInputLayout[i].InputSlot;
		dxInputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION(mInputLayout[i].InputSlotClass);
		dxInputLayout[i].InstanceDataStepRate = mInputLayout[i].InstanceDataStepRate;
		dxInputLayout[i].SemanticIndex = mInputLayout[i].SemanticIndex;
		dxInputLayout[i].SemanticName = mInputLayout[i].SemanticName;
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
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	mPso.dxPSO = psoDesc;
	mPso.PSOName = "Bloom";
	return mPso;
#endif
	return PipelineState();
}

PipelineState FPSO::BuildBloomSetFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader)
{
#ifdef _WIN32
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	dxInputLayout.resize(mInputLayout.size());
	for (int i = 0; i < mInputLayout.size(); i++)
	{
		dxInputLayout[i].AlignedByteOffset = mInputLayout[i].AlignedByteOffset;
		dxInputLayout[i].Format = DXGI_FORMAT(mInputLayout[i].Format);
		dxInputLayout[i].InputSlot = mInputLayout[i].InputSlot;
		dxInputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION(mInputLayout[i].InputSlotClass);
		dxInputLayout[i].InstanceDataStepRate = mInputLayout[i].InstanceDataStepRate;
		dxInputLayout[i].SemanticIndex = mInputLayout[i].SemanticIndex;
		dxInputLayout[i].SemanticName = mInputLayout[i].SemanticName;
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
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	mPso.PSOName = "BloomSet";
	mPso.dxPSO = psoDesc;
	return mPso;
#endif
	return PipelineState();
}

PipelineState FPSO::BuildBloomUpFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader, const std::string& PSOType)
{

#ifdef _WIN32
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	dxInputLayout.resize(mInputLayout.size());
	for (int i = 0; i < mInputLayout.size(); i++)
	{
		dxInputLayout[i].AlignedByteOffset = mInputLayout[i].AlignedByteOffset;
		dxInputLayout[i].Format = DXGI_FORMAT(mInputLayout[i].Format);
		dxInputLayout[i].InputSlot = mInputLayout[i].InputSlot;
		dxInputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION(mInputLayout[i].InputSlotClass);
		dxInputLayout[i].InstanceDataStepRate = mInputLayout[i].InstanceDataStepRate;
		dxInputLayout[i].SemanticIndex = mInputLayout[i].SemanticIndex;
		dxInputLayout[i].SemanticName = mInputLayout[i].SemanticName;
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
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	mPso.PSOName = PSOType;
	mPso.dxPSO = psoDesc;
	return mPso;
#endif
	return PipelineState();
}

PipelineState FPSO::BuildBloomDownFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader)
{
#ifdef _WIN32
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	dxInputLayout.resize(mInputLayout.size());
	for (int i = 0; i < mInputLayout.size(); i++)
	{
		dxInputLayout[i].AlignedByteOffset = mInputLayout[i].AlignedByteOffset;
		dxInputLayout[i].Format = DXGI_FORMAT(mInputLayout[i].Format);
		dxInputLayout[i].InputSlot = mInputLayout[i].InputSlot;
		dxInputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION(mInputLayout[i].InputSlotClass);
		dxInputLayout[i].InstanceDataStepRate = mInputLayout[i].InstanceDataStepRate;
		dxInputLayout[i].SemanticIndex = mInputLayout[i].SemanticIndex;
		dxInputLayout[i].SemanticName = mInputLayout[i].SemanticName;
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
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	mPso.PSOName = "BloomDown";
	mPso.dxPSO = psoDesc;
	return mPso;
#endif
	return PipelineState();
}
