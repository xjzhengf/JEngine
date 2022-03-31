#include "stdafx.h"
#include "DXRHIResource.h"
#include "DX12RHI.h"
#include "ShaderManager.h"

DXRHIResource::DXRHIResource()
{
	mResource = std::make_shared<FResource>();
}

std::shared_ptr<FResource> DXRHIResource::BackBuffer()
{
	mResource->Resource = DX12RHI::GetDX12RHI()->CurrentBackBuffer();
	return mResource;
}

unsigned __int64 DXRHIResource::CurrentBackBufferViewHand()
{
	unsigned __int64 ptr = DX12RHI::GetDX12RHI()->CurrentBackBufferView().ptr;
	return ptr;
}

unsigned __int64 DXRHIResource::CurrentDepthStencilViewHand()
{
	unsigned __int64 ptr = DX12RHI::GetDX12RHI()->DepthStencilView().ptr;
	return ptr;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC DXRHIResource::CreatePSO(const std::string& Name)
{
	if (Name == "Scene") {
		return BuildRenderPSO();
	}
	if (Name == "ShadowMap") {
		return BuildDepthPSO();
	}
	return D3D12_GRAPHICS_PIPELINE_STATE_DESC();
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC DXRHIResource::BuildRenderPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(),(UINT)mInputLayout.size() };

	psoDesc.VS = {
		reinterpret_cast<BYTE*>(ShaderManager::GetShaderManager()->mvsByteCode->GetBufferPointer()),
		ShaderManager::GetShaderManager()->mvsByteCode->GetBufferSize()
	};
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(ShaderManager::GetShaderManager()->mpsByteCode->GetBufferPointer()),
		ShaderManager::GetShaderManager()->mpsByteCode->GetBufferSize()
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
	return psoDesc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC DXRHIResource::BuildDepthPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(),(UINT)mInputLayout.size() };

	psoDesc.VS = {
		reinterpret_cast<BYTE*>(ShaderManager::GetShaderManager()->mvsByteCode->GetBufferPointer()),
		ShaderManager::GetShaderManager()->mvsByteCode->GetBufferSize()
	};
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(ShaderManager::GetShaderManager()->mpsByteCode->GetBufferPointer()),
		ShaderManager::GetShaderManager()->mpsByteCode->GetBufferSize()
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
	return psoDesc;
}
